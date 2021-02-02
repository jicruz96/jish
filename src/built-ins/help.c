#include "../shell.h"
#include <unistd.h>
#include <stdio.h>

/**
 * help_help - helps help help
 **/
void help_help(void)
{
	int i;
	char *lines[] = {
		"help: help [builtin]",
		"\tDisplay helpful information about builtin commands.",
		NULL};

	for (i = 0; lines[i]; i++)
		_puts(lines[i]);
}

/**
 * builtin_help - custom help builtin
 * @args: arguments
 * Return: 0 on success | 1 if error encountered
 **/
int builtin_help(char *args[])
{
	int i, j;
	char error_msg[256], *str = "%s: %s: no help topics match %s\n";
	char *builtins[] = {"alias", "cd", "env", "exit", "help", "history",
						"setenv", "unsetenv", NULL};
	void (*builtin_helps[])(void) = {&help_alias, &help_cd, &help_env,
									 &help_exit, &help_help, &help_history,
									 &help_setenv, &help_unsetenv, NULL};

	if (args[1] == NULL)
	{
		printf("usage: help [builtin]\n");
		return (1);
	}

	for (i = 1; args[i]; i++)
	{
		for (j = 0; builtins[j]; j++)
			if (_strcmp(args[i], builtins[j]) == 0)
				break;

		if (builtins[j])
			builtin_helps[j]();
		else
		{
			sprintf(error_msg, str, shell.name, shell.lines, args[i]);
			write(STDERR_FD, error_msg, _strlen(error_msg));
			return (1);
		}
	}

	return (0);
}
