#include "../shell.h"
#define max(a, b) ((a) > (b) ? (a) : (b))

/**
 * _getenv - returns value of env var
 * @key: env var
 * Return: value of env var
 */
char *_getenv(char *key)
{
	int i, j, key_len = _strlen(key);

	/* loop through environment array */
	for (i = 0; environ[i]; i++)
	{
		/* find the equals sign */
		for (j = 0; environ[i][j] != '='; j++)
			;
		/* compare up to either environ '=' or length of the key */
		if (_strncmp(key, environ[i], max(j, key_len)) == 0)
			return (_strdup(environ[i] + key_len + 1));
	}

	return (NULL);
}
