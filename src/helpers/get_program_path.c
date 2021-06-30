#include "../shell.h"
#include <sys/stat.h>
#include <sys/errno.h>
#include <stdio.h>
#include <string.h>

/**
 * get_program_path - finds a program in the PATH
 * @program: program name (char *)
 * Return: full program path || just program if not found in path
 **/
char *get_program_path(char *program)
{
	char *PATH = NULL, *path = NULL, *buffer = NULL;
	struct stat file_info;
	int i;

	if (!program)
		return (NULL);

	for (i = 0; program[i]; i++) /* If input is a path, return copy of input */
		if (program[i] == '/')
		{
			if (stat(program, &file_info))
			{
				shell.status = ENOENT;
				return (NULL);
			}
			if (!(S_IXUSR & file_info.st_mode))
			{
				shell.status = EACCES;
				return (NULL);
			}
			return (_strdup(program));
		}

	/* otherwise, check the PATH */
	PATH = _getenv("PATH"), buffer = malloc(sizeof(char) * 256);
	while ((path = strtok((path ? NULL : PATH), ":")))
	{
		sprintf(buffer, "%s/%s", path, program);
		if (stat(buffer, &file_info) == 0)
		{
			if (!(S_IXUSR & file_info.st_mode))
				shell.status = EACCES, free(buffer), buffer = NULL;
			free(PATH);
			return (buffer);
		}
	}
	shell.status = ENOENT, free(PATH), free(buffer);
	return (NULL);
}
