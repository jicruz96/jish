#include "my_strings.h"
#include <stdlib.h>

/**
 * _strdup - custom strdup
 * @str: string to duplicate
 * Return: duplicate of string
 **/
char *_strdup(char *str)
{
	char *dup;

	if (str == NULL)
		return (NULL);

	dup = (char *)malloc(_strlen(str) + 1);
	return (_strcpy(dup, str));
}

/**
 * _strndup - custom strndup
 * @str: string to duplicate
 * @n: maximum number of bytes to duplicate
 * Return: duplicate of string
 **/
char *_strndup(char *str, int n)
{
	char *dup;

	if (str == NULL)
		return (NULL);

	dup = (char *)malloc(n + 1);
	return (_strncpy(dup, str, n));
}
