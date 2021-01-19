#include "my_strings.h"
#include <stdlib.h>


/**
 * _calloc_string_array - allocate memory and initialize to 0
 * @size: size of array
 * Return: pointer to array
 **/
char **_calloc_string_array(int size)
{
	char **new = malloc(sizeof(char *) * size);
	int i;

	for (i = 0; i < size; i++)
		new[i] = NULL;

	return (new);
}

/**
 * _realloc_string_array - does what it sounds like
 * @array: array
 * @is_malloced: determines whether inserted array must be freed
 * Return: reference to new chunk
 **/
char **_realloc_string_array(char **array, int is_malloced)
{
	char **new = NULL;
	int i, size = 2;

	/* if array is NULL, allocate memory for one string and return */
	if (!array)
	{
		new = malloc(sizeof(char *));
		*new = NULL;
		return (new);
	}

	/* calculate size of array */
	for (i = 0; array[i]; i++)
		size += 1;

	/* allocate memory for new array with two spaces more than previous */
	new = malloc(sizeof(char *) * size);

	/* copy over strings from array onto new */
	for (i = 0; array[i]; i++)
	{
		new[i] = _strdup(array[i]);
		if (is_malloced)
			free(array[i]);
	}
	/* init new values made to null */
	new[i] = NULL;
	new[i + 1] = NULL;
	if (is_malloced)
		free(array);
	return (new);
}

/**
 * free_string_array - frees string array
 * @array: array
 **/
void free_string_array(char **array)
{
	int i;

	for (i = 0; array[i]; i++)
		free(array[i]);
	free(array);
}
