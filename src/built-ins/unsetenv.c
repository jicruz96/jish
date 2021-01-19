#include "../shell.h"
#include <string.h>
#include <unistd.h>

/**
 * builtin_unsetenv- custom unsetenv built-in. deletes environment variable
 * @cmd: command struct
 * Return: exit status
 **/
int builtin_unsetenv(command_t *cmd)
{
	int i, j;
	char *UNSET_VARIABLE, *var, **args = cmd->args;

	/* if no arguments were given, print error */
	if (args[1] == NULL)
	{
		write(STDERR_FILENO, "Usage: unsetenv VARIABLE\n", 25);
		return (1);
	}

	UNSET_VARIABLE = args[1];

	for (i = 0; environ[i]; i++)
	{
		var = _strdup(environ[i]);

		/* if the environment variable matches the target... */
		if (_strcmp(strtok(var, "="), UNSET_VARIABLE) == 0)
		{
			free(var);
			free(environ[i]);
			/* adjust the environment array */
			for (j = i + 1; environ[j]; i++, j++)
				environ[i] = environ[j];

			/* null-terminate the environment array */
			environ[i] = NULL;
			return (EXIT_SUCCESS);
		}
		free(var);
	}
	return (EXIT_SUCCESS);
}

/**
 * help_unsetenv - prints unsetenv help content
 **/
void help_unsetenv(void)
{
	int i;
	char *lines[] = {
		"unsetenv: unsetenv [VARIABLE]",
		"\tDeletes an environment variable",
		NULL};

	for (i = 0; lines[i]; i++)
		_puts(lines[i]);
}
