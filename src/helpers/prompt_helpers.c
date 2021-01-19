#include "../shell.h"
#include <time.h>
#include <unistd.h>

/**
 * get_cwd_prompt - gets current working directory for prompt formatting
 * Return: cwd as string
 **/
char *get_cwd_prompt(void)
{
	char *cwd = malloc(sizeof(char) * 256);
	char *home;

	getcwd(cwd, 256);
	home = _getenv("HOME");
	if (_strcmp(cwd, home) == 0)
	{
		free(cwd);
		cwd = _strdup("~");
	}
	free(home);
	return (cwd);
}
/**
 * get_shellname_prompt - returns shell name for prompt formatted string
 * Return: shellname as string
 **/
char *get_shellname_prompt(void)
{
	return (_strdup(shell.name));
}

/**
 * get_username_prompt - returns username for prompt formatted string
 * Return: username as string
 **/
char *get_username_prompt(void)
{
	return (_getenv("USER"));
}

/**
 * get_hostname_prompt - returns hostname for prompt formatted string
 * Return: hostname as string
 **/
char *get_hostname_prompt(void)
{
	char *name = malloc(sizeof(char) * 256);

	gethostname(name, 256);
	return (name);
}

/**
 * get_date_prompt - returns date from prompt formatted string
 * Return: date as string in (Weekday Month Date) format
 **/
char *get_date_prompt(void)
{
	time_t timer;
	char *date;

	time(&timer);
	date = ctime(&timer);
	return (_strndup(date, 10));
}
