#include "../shell.h"
#include <unistd.h>
#include <stdio.h>

/**
 * builtin_cd - custom cd (i.e. "change directory") built-in
 * @args: arguments
 * Return: exit status
 **/
int builtin_cd(char *args[])
{
	char cwd[256], *str = "%s: %d: %s: can't cd to %s\n";
	char error_msg[256], *OLDPWD[] = {0, "OLDPWD", 0}, *new[] = {0, "PWD", 0};
	int print_dir = 0;

	/* set OLDPWD to current working directory */
	OLDPWD[2] = getcwd(cwd, 256);

	/* set new directory target */
	if (_strcmp(args[1], "-") == 0)
	{
		new[2] = _getenv("OLDPWD");
		print_dir = true;
	}
	else
	{
		new[2] = args[1];
		if (!new[2] || !new[2][0])
			new[2] = _getenv("HOME");
	}

	/* if no home dir found, just stay in cwd! */
	if (!new[2] || !new[2][0])
		new[2] = OLDPWD[2];

	if (chdir(new[2]) == -1)
	{
		sprintf(error_msg, str, shell.name, shell.lines, "cd", new[2]);
		write(STDERR_FD, error_msg, _strlen(error_msg));
		return (2);
	}

	if (print_dir)
		write(STDOUT_FD, new[2], _strlen(new[2])), write(STDOUT_FD, "\n", 1);

	/* update environment */
	_setenv("PWD", new[2]);
	_setenv("OLDPWD", OLDPWD[2]);
	return (EXIT_SUCCESS);
}

/**
 * help_cd - prints cd help content
 **/
void help_cd(void)
{
	int i;
	char *lines[] = {
		"cd: cd [dir]",
		"\tChange the current directory to DIR.  The variable $HOME is the",
		"\tdefault DIR. If DIR is `-', then cd to the value of $OLDPWD",
		NULL};

	for (i = 0; lines[i]; i++)
		_puts(lines[i]);
}
