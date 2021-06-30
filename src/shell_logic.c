#include "shell.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/errno.h>

#define IS_FD_REDIR(x) (IS_NUMERIC(*(x)) && (x)[1] == '>')
#define AND_BREAK(status, logic) ((status) && ((logic) & IS_AND))
#define OR_BREAK(status, logic) (!(status) && ((logic) & IS_OR))
#define LOGIC_BREAK(a, b) (AND_BREAK(a, b) || OR_BREAK(a, b))


/**
 * run_shell - executes a file line by line
 * @fd: file descriptor
 * Return: exit status
 **/
void run_shell(int fd)
{
	char *tokens[256];
	int i;

	for (i = 0; i < 256; i++)
		tokens[i] = NULL;

	while (shell.run)
	{
		/* get and write shell prompt */
		print_prompt(fd);
		signal(SIGINT, print_prompt);

		/* respond to user inputs */
		if (get_tokens(tokens, fd) == -1)
			break;

		/* execute line */
		execute_line(tokens);
		shell.lines++;
	}

	get_tokens(NULL, -1);
}

/**
 * execute_line - executes a line of tokens
 * @tokens: tokens array
 * Return: cmd of command_t list
 */
void execute_line(char **tokens)
{
	int in_cpy = dup(STDIN_FD), out_cpy = dup(STDOUT_FD), prev_logic = 0, i;
	command_t cmd;

	for (i = 0; i < 256; i++)
		cmd.args[i] = NULL;

	while (*tokens && shell.run)
	{
		if (IS_SEPARATOR(*tokens) || IS_REDIR_TOKEN(*tokens))
		{
			shell.status = handle_syntax_error(*tokens);
			break;
		}

		tokens = command_config(&cmd, tokens);

		if (get_IO(&cmd, prev_logic) == 1)
		{
			dup2(cmd.input_fd, STDIN_FD), dup2(cmd.output_fd, STDOUT_FD);
			shell.status = cmd.executor(&cmd);
			dup2(in_cpy, STDIN_FD), dup2(out_cpy, STDOUT_FD);
		}

		if (cmd.input_fd > 2)
			close(cmd.input_fd);
		if (cmd.output_fd > 2)
			close(cmd.output_fd);

		free(cmd.path), free(cmd.in_name), free(cmd.out_name);
		for (i = 0; cmd.args[i]; i++)
			free(cmd.args[i]), cmd.args[i] = NULL;

		prev_logic = cmd.logic;

		if (LOGIC_BREAK(shell.status, cmd.logic))
			break;
	}

	while (*tokens)
		free(*tokens++);
}


/**
 * command_config - configures command struct
 * @cmd: pointer to command struct
 * @tokens: tokens
 * Return: remaining unused tokens
 */
char **command_config(command_t *cmd, char **tokens)
{
	char *command = replace_vars(get_alias(*tokens++));
	int i = 0;

	cmd->logic    = DEFAULT_LOGIC;
	cmd->args[0]  = command;
	cmd->in_name  = NULL, cmd->input_fd  = STDIN_FD;
	cmd->out_name = NULL, cmd->output_fd = STDOUT_FD;
	cmd->err_name = NULL, cmd->error_fd  = STDERR_FD;
	tokens        = parse_tokens(cmd, tokens);

	for (i = 0; shell.builtins[i].name; i++)
		if (_strcmp(command, shell.builtins[i].name) == 0)
		{
			cmd->path     = _strdup(command);
			cmd->executor = shell.builtins[i].function;
			return (tokens);
		}

	cmd->path     = get_program_path(command);
	cmd->executor = cmd->path ? &fork_and_execute : &handle_error;
	return (tokens);
}

/**
 * parse_tokens - parses tokens and applies configuration to command struct
 * @cmd: command struct
 * @tokens: tokens
 * Return: remaining tokens
 **/
char **parse_tokens(command_t *cmd, char **tokens)
{
	int separator_logic[] = {IS_AND, IS_OR, IS_PIPE, DEFAULT_LOGIC}, i;
	char *separators[] = {"&&", "||", "|", ";", NULL};

	for (i = 1; !IS_SEPARATOR(*tokens); tokens++)
		if (!_strcmp(*tokens, "<"))
		{
			free(*tokens++), cmd->logic   |= IS_REDIR_IN;
			cmd->in_name  = replace_vars(*tokens);
		}
		else if (!_strcmp(*tokens, "<<"))
		{
			free(*tokens++), cmd->logic   |= IS_HEREDOC;
			cmd->in_name  = replace_vars(*tokens);
		}
		else if (!_strcmp(*tokens, ">"))
		{
			free(*tokens++), cmd->logic   |= IS_REDIR_OUT;
			cmd->out_name = replace_vars(*tokens);
		}
		else if (!_strcmp(*tokens, ">>"))
		{
			free(*tokens++), cmd->logic   |= IS_APPEND;
			cmd->out_name = replace_vars(*tokens);
		}
		else if (IS_FD_REDIR(*tokens))
		{
			free(*tokens++), cmd->logic   |= IS_REDIR_OUT;
			cmd->out_name = replace_vars(*tokens);
		}
		else
			cmd->args[i++] = replace_vars(*tokens);

	for (i = 0; separators[i]; i++)
		if (!_strcmp(*tokens, separators[i]))
		{
			free(*tokens++), cmd->logic |= separator_logic[i];
			return (tokens);
		}

	return (tokens);
}

/**
 * fork_and_execute - function wrapper for fork/execve
 * @cmd: command node
 * Return: exit status of execution
 **/
int fork_and_execute(command_t *cmd)
{
	pid_t child_pid;
	int status = 0;

	child_pid = fork();

	/* child executes */
	if (child_pid == 0)
	{
		shell.status = execve(cmd->path, cmd->args, environ);
		exit(handle_error(cmd));
	}

	/* Parent waits and returns status */
	if (child_pid == -1 || waitpid(child_pid, &status, 0) == -1)
		return (handle_error(cmd));
	else
		return (WEXITSTATUS(status));
}
