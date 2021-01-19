/**
 * _strlen - custom strlen
 * @str: string
 * Return: length
 **/
int _strlen(char *str)
{
	int i = 0;

	if (str)
		while (*(str++))
			i++;

	return (i);
}
