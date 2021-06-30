#include "shell.h"
#include <fcntl.h>
#include <sys/errno.h>
#include <unistd.h>
#include <stdio.h>

/**
 * get_IO - formats cmd input and output streams
 * @cmd: command node
 * @prev_logic: previous command's logic
 * Return: 1 on success -1 on failure
 */
int get_IO(command_t *cmd, int prev_logic)
{
	static int pipefds[2];

	if (prev_logic & IS_PIPE) /* get input_fd */
		cmd->input_fd = pipefds[0];
	else
		cmd->input_fd = get_input_fd(cmd);
	if (cmd->input_fd == -1)
		return (-1);

	cmd->output_fd = get_output_fd(cmd); /* get output fd */

	if (cmd->logic & IS_PIPE && !(cmd->logic & (IS_REDIR_OUT | IS_APPEND)))
		pipe(pipefds), cmd->output_fd = pipefds[1];
	if (cmd->output_fd == -1)
		return (-1);
	return (1);
}

/**
 * get_output_fd - gets output fd
 * @cmd: command node
 * Return: output file descriptor
 **/
int get_output_fd(command_t *cmd)
{
	int fd, perms;
	char error_msg[256], *str = "%s: %d: cannot open %s: ";

	if (cmd->logic & (IS_REDIR_OUT | IS_APPEND))
	{
		if (cmd->out_name == NULL)
		{
			fd = handle_syntax_error(cmd->logic & IS_APPEND ? ">>" : ">");
			shell.status = fd; /* this just exists to get prev line under 80 */
			return (-1);
		}
		if (cmd->logic & IS_APPEND)
			perms = O_CREAT | O_RDWR | O_APPEND;
		else
			perms = O_CREAT | O_RDWR | O_TRUNC;

		fd = open(cmd->out_name, perms, 0644);
		if (fd == -1)
		{
			shell.status = 2;
			sprintf(error_msg, str, shell.name, shell.lines, cmd->out_name);
			if (errno == ENOENT)
				_strcat(error_msg, "No such file\n");
			else
				_strcat(error_msg, "Permission denied\n");
			write(STDERR_FD, error_msg, _strlen(error_msg));
			return (-1);
		}
		return (fd);
	}
	return (STDOUT_FD);
}

/**
 * get_input_fd - get input fd
 * @cmd: command node
 * Return: input file descriptor
 **/
int get_input_fd(command_t *cmd)
{
	int pipefds[2], fd;
	char error_msg[256], *str = "%s: %d: cannot open %s: ";

	if (cmd->logic & IS_REDIR_IN)
	{
		if (cmd->in_name == NULL)
		{
			shell.status = handle_syntax_error("<");
			return (-1);
		}

		fd = open(cmd->in_name, O_RDONLY);
		if (fd == -1)
		{
			shell.status = 2;
			sprintf(error_msg, str, shell.name, shell.lines, cmd->in_name);
			if (errno == ENOENT)
				_strcat(error_msg, "No such file\n");
			else
				_strcat(error_msg, "Permission denied\n");
			write(STDERR_FD, error_msg, _strlen(error_msg));
			return (-1);
		}
		return (fd);
	}

	if ((cmd->logic & IS_HEREDOC) && cmd->args[1] == NULL)
	{
		/* make pipe for heredoc */
		pipe(pipefds);
		/* write contents onto pipe */
		if (cmd->in_name)
			write(pipefds[1], cmd->in_name, _strlen(cmd->in_name));
		/* close writing side of pipe */
		close(pipefds[1]);
		/* set input to reading side of pipe */
		return (pipefds[0]);
	}

	return (STDIN_FD);
}
