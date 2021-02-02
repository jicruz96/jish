#include "../shell.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

/**
 * builtin_history - custom history built-in
 * @args: arguments
 * Return: exit status
 **/
int builtin_history(char *args[])
{
	int i;

	(void)args;
	for (i = 0; shell.history[i]; i++)
		printf("%5d  %s", i, shell.history[i]);

	return (0);
}

/**
 * help_history - prints history help content
 **/
void help_history(void)
{
	int i;
	char *lines[] = {
		"history:",
		"\tPrints history",
		NULL};

	for (i = 0; lines[i]; i++)
		_puts(lines[i]);
}

/**
 * get_history - saves shell history to history array
 * @history: array where history will be saved
 * Return: history size
 **/
int get_history(char *history[])
{
	/*char path[256], *line = NULL;*/
	/*int i = 0, history_fd, line_size = 0;*/

	/* create path to history file */
	(void)history;
	return (0);
	/*sprintf(path, "%s/%s", _getenv("HOME"), ".hsh_history");*/

	/*history_fd = open(path, O_CREAT | O_RDONLY, 0644);*/

	/*for (i = 0; _getline(&line, &line_size, history_fd) > 0; i++)*/
	/*{*/
	/*	if (i == HISTSIZE)*/
	/*		while (i)*/
	/*			free(history[i]), history[i--] = NULL;*/
	/*	history[i] = _strdup(line);*/
	/*}*/

	/*close(history_fd);*/
	/*free(line);*/
	/*return (i);*/
}

/**
 * save_line_to_history - saves a line to history array
 * @line: line to save in array
 **/
void save_line_to_history(char *line)
{
	static int end_index = HISTSIZE - 1;
	static int current_index;
	static int history_size = HISTSIZE;
	int i;
	char **tmp;

	if (current_index == 0)
		current_index = shell.history_size;

	if (current_index > end_index)
	{
		history_size += HISTSIZE;
		tmp = malloc(sizeof(char) * (history_size));
		for (i = 0; i < history_size; i++)
			if (i >= end_index)
				tmp[i] = NULL;
			else
				tmp[i] = shell.history[i];
		free(shell.history);
		shell.history = tmp;
		end_index = history_size - 2;
	}
	shell.history[current_index] = line;
	current_index += 1;
}

/**
 * save_history_to_file - saves a line to history array
 **/
void save_history_to_file(void)
{
	int i, fd;
	char history_path[256];

	sprintf(history_path, "%s/%s", _getenv("HOME"), ".hsh_history");
	fd = open(history_path, O_WRONLY | O_APPEND);
	for (i = 0; shell.history[i]; i++)
	{
		if (i >= shell.history_size)
			write(fd, shell.history[i], _strlen(shell.history[i]));
		free(shell.history[i]);
	}
	free(shell.history);
	close(fd);
}
