#include "_a.h"
#include <fcntl.h>
#include <sys/errno.h>
#include <unistd.h>
#include <stdio.h>

shell_t shell = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/**
 * main - entry point to shell
 * @argc: arg count
 * @argv: argument array
 * Return: exit status
 **/
int main(int argc, char *argv[])
{
	int fd = STDIN_FD;
	char *shellname = argv[0], error_msg[256];
	builtin_t builtins[] = {
		{"cd",          &builtin_cd},       {"exit",        &builtin_exit},
		{"env",         &builtin_env},      {"setenv",      &builtin_setenv},
		{"unsetenv",    &builtin_unsetenv}, {"alias",       &builtin_alias},
		{"history",     &builtin_history},  {"help",        &builtin_help},
		{NULL,          NULL}
	};

	if (argc > 1) /* get input/shellname if argument was passed */
	{
		fd = open(argv[1], O_RDONLY);
		if (fd == -1)	/* exit if input does not exist */
		{
			sprintf(error_msg, "%s: 0: Can't open %s\n", argv[0], argv[1]);
			write(STDERR_FD, error_msg, _strlen(error_msg));
			return (CANT_OPEN);
		}
		shellname = argv[1];
	}

	shell_init(shellname, fd, builtins);
	execute_hshrc();
	run_shell(fd);
	shell_cleanup();

	return (shell.status);
}

/**
 * shell_init - initializes shell
 * @shellname: shell name
 * @input: shell input
 * @builtins: array of known shell builtin names and associated functions
 **/
void shell_init(char *shellname, int input, builtin_t *builtins)
{
	shell.lines            = 1;
	shell.run              = true;
	shell.name             = shellname;
	shell.builtins         = builtins;
	shell.interactive      = isatty(input);
	environ                = _realloc_string_array(environ, false);
	if (shell.interactive)
	{
		shell.history      = _calloc_string_array(HISTSIZE);
		shell.history_size = get_history(shell.history);
	}
}

/**
 * shell_cleanup - performs memory cleanups
 **/
void shell_cleanup(void)
{
	int i;
	alias_t *tmp;

	/* Save history to history file */
	if (shell.interactive)
		save_history_to_file();

	/* free environ array */
	for (i = 0; environ[i]; i++)
		free(environ[i]);
	free(environ);

	/* free aliases */
	while (shell.aliases)
	{
		tmp = shell.aliases;
		shell.aliases = shell.aliases->next;
		free(tmp->alias), free(tmp->value), free(tmp);
	}
}

/**
 * execute_hshrc - executes the .hshrc file if it exists
 * Return: exit status of run_shell()
 **/
void execute_hshrc(void)
{
	int hshrc_fd;
	char *homedir, path[256]/*, error_msg[256]*/;

	/* create path for hshrc */
	homedir = _getenv("HOME");
	sprintf(path, "%s/%s", homedir, ".hshrc");

	/* go get .hshrc file descriptor */
	hshrc_fd = open(path, O_RDONLY);
	if (hshrc_fd == -1)
	{
		if (errno != ENOENT)
		{
			fprintf(stderr, "%s: 0: Can't open %s\n", shell.name, path);
			/*write(STDERR_FD, error_msg, _strlen(error_msg));*/
		}
	}
	else
	{
		run_shell(hshrc_fd);
	}

}
