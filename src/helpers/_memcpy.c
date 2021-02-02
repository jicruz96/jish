#include "../shell.h"

/**
 * _memcpy - custom memcpy
 * @dest: memory area to copy to
 * @src: memory area to copy from
 * @n: number of bytes to set
 * Return: pointer to dest
 **/
char *_memcpy(char *dest, char *src, int n)
{
	int i;

	for (i = 0; i < n; i++)
		dest[i] = src[i];

	return (dest);
}
