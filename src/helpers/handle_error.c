#include "../shell.h"
#include <sys/errno.h>
#include <unistd.h>
#include <stdio.h>

/**
 * handle_error - prints error messages
 * @cmd: command node
 * Return: error code
 **/
int handle_error(command_t *cmd)
{
	char error[256], *msg, *command = cmd->args[0];

	if (shell.status == ENOENT)
		msg = "not found", shell.status = 127;
	else if (shell.status == EACCES)
		msg = "Permission denied", shell.status = 126;
	else
		msg = "Unknown error. Inspect exit status.", shell.status = errno;

	sprintf(error, "%s: %d: %s: %s\n", shell.name, shell.lines, command, msg);
	write(STDERR_FD, error, _strlen(error));
	return (shell.status);
}

/**
 * handle_syntax_error - function wrapper for syntax error handling
 * @token: culprit token
 * Return: status code
 **/
int handle_syntax_error(char *token)
{
	char error[256], tmp[8];
	char *str = "%s: %d: Syntax error: %s unexpected\n";

	if (IS_SEPARATOR(token))
		sprintf(tmp, "\"%s\"", token);
	else
		_strcpy(tmp, "newline");

	sprintf(error, str, shell.name, shell.lines, tmp);
	write(STDERR_FD, error, _strlen(error));
	return (2);
}
