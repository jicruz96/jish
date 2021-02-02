#include "shell.h"
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>

/**
 * execute_commands - executes commands in a command chain
 * @cmd: head of command chain
 **/
void execute_commands(command_t *cmd)
{
	int prev_logic = DEFAULT_LOGIC, input_copy = dup(STDIN_FD), output_copy;
	command_t *tmp_node;
	exec_f executor;


	while (cmd && shell.run)
	{
		if (get_IO(cmd, prev_logic) == -1)
		{
			shell.status = errno;
			shell.status = handle_error(cmd->args);
		}
		else
		{
			setup_args(cmd->args);
			executor = get_executor(cmd->args[0]);
			output_copy = dup(cmd->redirect_input);
			dup2(cmd->input_fd, STDIN_FD), dup2(cmd->output_fd, cmd->redirect_input);
			shell.status = executor(cmd->args);
			close(cmd->input_fd), close(cmd->output_fd);
			dup2(input_copy, STDIN_FD),	dup2(output_copy, cmd->redirect_input);
		}
		prev_logic = cmd->logic;
		while (shell.open_vars_i)
		{
			free(shell.open_vars[--shell.open_vars_i]);
			shell.open_vars[shell.open_vars_i] = NULL;
		}
		tmp_node = cmd->next;
		free(cmd);
		cmd = tmp_node;
		if (shell.status && IS_AND(prev_logic))
			break;
		if (!shell.status && IS_OR(prev_logic))
			break;
		if (IS_PIPE(prev_logic) && IS_REDIR_OUT(prev_logic))
			break;
	}
	free_command_chain(&cmd);
}

/**
 * get_IO - function wrapper for get_input and get_output
 * @cmd: command
 * @prev_logic: previous command's logic (needed for get_input)
 * Return: 0 on success | -1 on failure
 **/
int get_IO(command_t *cmd, int prev_logic)
{
	static int pipefds[2];

	cmd->input_fd = get_input(cmd, (int **)&pipefds, prev_logic);
	if (cmd->input_fd == -1)
		return (-1);
	cmd->output_fd = get_output(cmd, (int **)&pipefds);
	if (cmd->output_fd == -1)
		return (-1);
	return (0);
}

/**
 * get_output - get output file descriptor of a command
 * @cmd: command
 * @pipefds: pointer to pipefds array
 * Return: output file descriptor | -1 on failure
 **/
int get_output(command_t *cmd, int **pipefds)
{
	int perms;

	/* Set up output */
	if (cmd->redirect)
	{
		perms = O_CREAT | O_WRONLY | (IS_APPEND(cmd->logic) ? O_APPEND : O_TRUNC);
		replace_vars(&(cmd->redirect));
		return (open(cmd->redirect, perms, 0644));
	}
	else if (IS_PIPE(cmd->logic))
	{
		pipe(*pipefds);
		return ((*pipefds)[1]);
	}
	else
	{
		return (dup(STDOUT_FD));
	}

}
/**
 * get_input - get input file descriptor for a command
 * @cmd: command
 * @pipefds: pointer to pipe
 * @prev_logic: previous command's logic (needed to determine input)
 * Return: input file descriptor | -1 on failure
 **/
int get_input(command_t *cmd, int **pipefds, int prev_logic)
{
	if (IS_HEREDOC(cmd->logic))
	{
		/* Close previous pipe if it was open */
		if (IS_PIPE(prev_logic))
			close((*pipefds)[0]);

		/* Make new pipe */
		pipe(*pipefds);

		/* Write input into pipe */
		write((*pipefds)[1], cmd->input, _strlen(cmd->input));

		/* Close writing side of pipe */
		close((*pipefds)[1]);

		/* Set input_fd to reading side of pipe */
		return ((*pipefds)[0]);
	}

	if (cmd->input)
	{
		if (IS_PIPE(prev_logic))
			close((*pipefds)[1]);
		replace_vars(&(cmd->input));
		return (open(cmd->input, O_RDONLY));
	}

	if (IS_PIPE(prev_logic))
		return ((*pipefds)[0]);

	return (dup(STDIN_FD));
}

/**
 * setup_args - replaces variables and removes double quotes for each argument
 *              in a list of arguments
 * @args:       arguments list
 **/
void setup_args(char *args[])
{
	int i;

	for (i = 0; args[i]; i++)
	{
		fix_dquote(args[i]);
		replace_vars(args + i);
	}
}
