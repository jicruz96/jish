#include <stdlib.h>

/**
 * _realloc - custom realloc
 * @p: pointer
 * @size: new size
 * Return: p
 **/
char *_realloc(char *p, int size)
{
	char *new;
	int i = 0;

	new = malloc(sizeof(char) * size);
	if (p)
		for (i = 0; p[i] && i < size; i++)
			new[i] = p[i];
	for (; i < size; i++)
		new[i] = '\0';
	free(p);
	return (new);
}
