#include "../shell.h"
#include <unistd.h>
#define max(a, b) ((a) > (b) ? (a) : (b))

/**
 * builtin_unsetenv- custom unsetenv built-in. deletes environment variable
 * @args: arguments
 * Return: exit status
 **/
int builtin_unsetenv(char *args[])
{

	if (args[1] == NULL)
	{
		write(STDERR_FILENO, "Usage: unsetenv VARIABLE\n", 25);
		return (1);
	}

	return (_unsetenv(args[1]));
}

/**
 * _unsetenv - custom unsetenv
 * @key: env variable to unset
 * Return: 0
 **/
int _unsetenv(char *key)
{
	int i, j, key_len = _strlen(key);

	for (i = 0; environ[i]; i++)
	{
		for (j = 0; environ[i][j]; j++)
			if (environ[i][j] == '=')
				break;

		/* if the environment variable matches the target... */
		if (_strncmp(environ[i], key, max(j, key_len)) == 0)
		{
			free(environ[i]);

			/* adjust the environment array */
			for (j = i + 1; environ[j]; i++, j++)
				environ[i] = environ[j];

			/* null-terminate the environment array */
			environ[i] = NULL;
			return (EXIT_SUCCESS);
		}
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
