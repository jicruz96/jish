#include "../shell.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <fcntl.h>


/**
 * get_executor - gets executor
 * @command: command
 * Return: pointer to executor function
 **/
exec_f get_executor(char *command)
{
	int i;

	for (i = 0; shell.builtins[i].name; i++)
		if (_strcmp(command, shell.builtins[i].name) == 0)
			return (shell.builtins[i].function);

	return (&fork_and_execute);
}

/**
 * get_path - finds a file in the path
 * @path_buffer: buffer
 * @file: file name
 * Return: 0 on success | -1 on error (inspect shell.status)
 **/
int get_path(char *path_buffer, char *file)
{
	char *path;
	struct stat file_info;
	int i = 0;

	if (!file)
		return (-1);
	for (i = 0; file[i]; i++)
		if (file[i] == '/')
		{
			if (stat(file, &file_info))
			{
				shell.status = ENOENT, *path_buffer = '\0';
				return (-1);
			}
			if (!(S_IXUSR & file_info.st_mode))
			{
				shell.status = EACCES, *path_buffer = '\0';
				return (-1);
			}
			_strcpy(path_buffer, file);
			return (0);
		}

	for (path = _getenv("PATH"); path && *path; path += i + (path[i] != 0))
	{
		for (i = 0; path[i] && path[i] != ':'; i++)
			;
		sprintf(path_buffer, "%.*s/%s", i, path, file);
		if (stat(path_buffer, &file_info) == 0)
		{
			if (!(S_IXUSR & file_info.st_mode))
			{
				shell.status = EACCES, *path_buffer = '\0';
				return (-1);
			}
			return (0);
		}
	}
	shell.status = ENOENT, *path_buffer = '\0';
	return (-1);
}


/**
 * fork_and_execute - function wrapper for fork/execve
 * @args:             arguments array
 * Return:            exit status of execution
 **/
int fork_and_execute(char *args[])
{
	pid_t child_pid;
	int status = 0;
	char path[256];

	if (!args || get_path(path, args[0]) == -1)
		return (handle_error(args));

	child_pid = fork();

	/* child executes */
	if (child_pid == 0)
	{
		shell.status = execve(path, args, environ);
		exit(handle_error(args));
	}

	/* Parent waits and returns status */
	if (child_pid == -1 || waitpid(child_pid, &status, 0) == -1)
		return (handle_error(args));
	else
		return (WEXITSTATUS(status));
}

/**
 * get_heredoc - gets heredoc
 * @end_tag: end tag that signals end of heredoc
 * @fd: file descriptor (to get more lines if needed)
 * Return: heredoc
 **/
char *get_heredoc(char *end_tag, int fd)
{
	char *token, *tmp = _realloc(NULL, sizeof(char) * (READ_SIZE + 1));
	int tmp_size = READ_SIZE + 1, token_length = 0, i;
	int end_tag_len = 0;
	int a;

	if (end_tag == NULL)
		return (NULL);

	end_tag_len = _strlen(end_tag);
	token = malloc(sizeof(char) * 1);
	*token = '\0';
	while (true)
	{
		/* print PS2 prompt if stdin is terminal */
		if (isatty(fd))
			write(fd, PS2, _strlen(PS2));

		/* get next line */
		i = 0;
		while (read(fd, tmp + i, 1) > 0)
		{
			a = false;
			if (tmp[i] == '\n')
				break;
			increase_buffer(&tmp, &tmp_size, ++i);
		}

		/* if next line is end tag or EOF, clean up and exit loop */
		if (a || _strncmp(tmp, end_tag, end_tag_len) == 0)
			break;

		/* else, make space and concat tmp onto token */
		a = true;
		token_length += i;
		token = _realloc(token, token_length + 1);
		_strncat(token, tmp, i + 1);
	}
	free(tmp);
	shell.open_vars[shell.open_vars_i++] = token;
	return (token);
}

/**
 * fix_dquote - remove surrounding double quotes from an argument
 * @arg: argument
 **/
void fix_dquote(char *arg)
{
	int i, j;

	if (*arg == '"')
	{
		for (i = 1, j = 0; arg[i] != '"'; i++, j++)
			arg[j] = arg[i];

		arg[i] = '\0';
	}
}
