#include "../shell.h"
#include <sys/stat.h>
#include <sys/errno.h>
#include <stdio.h>
#include <string.h>

/**
 * get_path - finds a file in the path
 * @path_buffer: buffer
 * @file: file name (char *)
 * Return: full file path || NULL if not found or no permission to execute
 **/
char *get_path(char *path_buffer, char *file)
{
	char *path;
	struct stat file_info;
	int i = 0;

	if (!file)
		return (NULL);

	for (i = 0; file[i]; i++)
		if (file[i] == '/')
		{
			if (stat(file, &file_info))
			{
				shell.status = ENOENT, *path_buffer = '\0';
				return (NULL);
			}
			if (!(S_IXUSR & file_info.st_mode))
			{
				shell.status = EACCES, *path_buffer = '\0';
				return (NULL);
			}
			return (_strcpy(path_buffer, file));
		}

	for (path = _getenv("PATH"); path && *path; path = path + i + (path[i] != 0))
	{
		for (i = 0; path[i] && path[i] != ':'; i++)
			;
		sprintf(path_buffer, "%.*s/%s", i, path, file);
		if (stat(path_buffer, &file_info) == 0)
		{
			if (!(S_IXUSR & file_info.st_mode))
			{
				shell.status = EACCES, *path_buffer = '\0';
				return (NULL);
			}
			return (path_buffer);
		}
	}
	shell.status = ENOENT, *path_buffer = '\0';
	return (NULL);
}
