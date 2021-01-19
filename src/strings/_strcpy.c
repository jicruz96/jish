/**
 * _strcpy - custom strcpy
 * @dest: where string will be copied
 * @src: string to copy
 * Return: pointer to dest
 **/
char *_strcpy(char *dest, char *src)
{
	int i;

	if (src)
	{
		for (i = 0; src[i]; i++)
			dest[i] = src[i];
		dest[i] = '\0';
	}
	return (dest);
}

/**
 * _strncpy - custom strncpy
 * @dest: where string will be copied
 * @src: string to copy
 * @n: maximum number of bytes to copy
 * Return: pointer to dest
 **/
char *_strncpy(char *dest, char *src, int n)
{
	int i;

	if (src)
	{
		for (i = 0; src[i] && i < n; i++)
			dest[i] = src[i];
		dest[i] = '\0';
	}
	return (dest);
}
