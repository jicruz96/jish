#include "../shell.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#define max(a, b) ((a) > (b) ? (a) : (b))

/**
 * builtin_setenv - custom setenv built-in. sets new environment variable
 * @args: arguments
 * Return: exit status
 **/
int builtin_setenv(char *args[])
{
	/* if only "setenv" passed in, call env */
	if (args[1] == NULL || args[2] == NULL)
	{
		printf("1: %s\n2: %s\n", args[1], args[2]);
		write(STDERR_FD, "Usage: setenv VARIABLE VALUE\n", 29);
		return (1);
	}

	/* retrieve our env variable and new corresponding value */
	return (_setenv(args[1], args[2]));
}

/**
 * _setenv - custom setenv
 * @key: environment variable
 * @value: value
 * Return: 0
 **/
int _setenv(char *key, char *value)
{
	char *ENV_VAR = malloc(sizeof(char) * 256);
	int i, j, key_len = _strlen(key);

	sprintf(ENV_VAR, "%s=%s", key, value);

	for (i = 0; environ[i]; i++)
	{
		for (j = 0; environ[i][j]; j++)
			if (environ[i][j] == '=')
				break;

		if (_strncmp(environ[i], key, max(j, key_len)) == 0)
		{
			free(environ[i]);
			environ[i] = ENV_VAR;
			return (EXIT_SUCCESS);
		}
	}
	/* set environ [i] to our new env var if it's NULL */
	environ = _realloc_string_array(environ, true);
	environ[i] = ENV_VAR;
	return (EXIT_SUCCESS);
}

/**
 * help_setenv - prints setenv help content
 **/
void help_setenv(void)
{
	int i;
	char *lines[] = {
		"setenv: setenv [VARIABLE] [VALUE]",
		"\tUpdate, or set a new enviornment variable.",
		"\tIf called with no arguements, a list of all",
		"\tenviornment variables will be printed, see env.",
		NULL};

	for (i = 0; lines[i]; i++)
		_puts(lines[i]);
}
