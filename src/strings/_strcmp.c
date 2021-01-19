/**
 * _strcmp - custom strcmp
 * @s1: first string
 * @s2: second string
 * Return: lexicographical difference between s1 and s2 | -1 if NULL
 **/
int _strcmp(char *s1, char *s2)
{
	int i;

	if (!s1 || !s2)
		return (-1);

	for (i = 0; s1[i] && s2[i] && s1[i] == s2[i]; i++)
		;

	return (s1[i] - s2[i]);
}

/**
 * _strncmp - custom strncmp
 * @s1: first string
 * @s2: second string
 * @n: max number of bytes to check
 * Return:  * lexicographical difference between s1's and s2's first n bytes
 *          * -1 if either input string is null
 **/
int _strncmp(char *s1, char *s2, int n)
{
	int i;

	if (!s1 || !s2)
		return (-1);

	for (i = 0; s1[i] && s2[i] && s1[i] == s2[i] && i < n - 1; i++)
		;

	return (s1[i] - s2[i]);
}
