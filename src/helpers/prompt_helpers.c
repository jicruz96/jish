#include "../shell.h"
#include <time.h>
#include <unistd.h>

/**
 * get_cwd_prompt - gets current working directory for prompt formatting
 * @buffer: buffer
 * Return: cwd as string
 **/
char *get_cwd_prompt(char *buffer)
{
	getcwd(buffer, 256);
	if (_strcmp(buffer, _getenv("HOME")) == 0)
		return (_strcpy(buffer, "~"));
	return (buffer);
}
/**
 * get_shellname_prompt - returns shell name for prompt formatted string
 * @buffer: buffer
 * Return: shellname as string
 **/
char *get_shellname_prompt(char *buffer)
{
	return (_strcpy(buffer, shell.name));
}

/**
 * get_username_prompt - returns username for prompt formatted string
 * @buffer: buffer
 * Return: username as string
 **/
char *get_username_prompt(char *buffer)
{
	return (_strcpy(buffer, _getenv("USER")));
}

/**
 * get_hostname_prompt - returns hostname for prompt formatted string
 * @buffer: buffer
 * Return: hostname as string
 **/
char *get_hostname_prompt(char *buffer)
{
	gethostname(buffer, 256);
	return (buffer);
}

/**
 * get_date_prompt - returns date from prompt formatted string
 * @buffer: buffer
 * Return: date as string in (Weekday Month Date) format
 **/
char *get_date_prompt(char *buffer)
{
	time_t timer;
	char *date;

	time(&timer);
	date = ctime(&timer);
	return (_strncpy(buffer, date, 10));
}
