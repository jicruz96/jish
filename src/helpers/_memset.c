#include "../shell.h"

/**
 * _memset - custom memset
 * @s: pointer to area to set
 * @c: character to set memory with
 * @n: number of bytes to set
 **/
void _memset(char *s, int c, int n)
{
	int i;

	for (i = 0; i < n; i++)
		s[i] = c;
}
