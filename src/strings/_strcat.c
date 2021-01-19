#include "my_strings.h"

/**
 * _strcat - custom strcat
 * @dest: destination buffer
 * @src: src string
 * Return: pointer to dest
 **/
char *_strcat(char *dest, char *src)
{
	if (src == NULL || dest == NULL)
		return (dest);

	_strcpy(dest + _strlen(dest), src);
	return (dest);
}

/**
 * _strncat - custom strncat
 * @dest: destination buffer
 * @src: src string
 * @n: maximum number of bytes to concatenate
 * Return: pointer to dest
 **/
char *_strncat(char *dest, char *src, int n)
{
	if (src == NULL || dest == NULL)
		return (dest);

	_strncpy(dest + _strlen(dest), src, n);
	return (dest);
}
