#include "../shell.h"

/**
 * builtin_ls - function wrapper for ls executable
 * @args: arguments
 * Return: exit status
 **/
int builtin_ls(char *args[])
{
	int i;

	for (i = 0; args[i]; i++)
		;

	args[i++] = _strdup("--color=auto");
	shell.open_vars[shell.open_vars_i++] = args[i - 1];
	args[i] = NULL;
	return (fork_and_execute(args));
}
