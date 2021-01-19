#include "../shell.h"
#include <stdio.h>
#include <unistd.h>

/**
 * replace_vars - detects an replaces variables in a shell token
 * @token: token
 * Return: token with all variables replaces
 **/
char *replace_vars(char *token)
{
	char *new_token, *value;
	int i;

	if (!token)
		return (NULL);

	/* check for a '$' . If no dollar signs, return token */
	for (i = 0; token[i] != '$'; i++)
		if (token[i] == '\0')
		{
			new_token = _strdup(token), free(token);
			return (new_token);
		}

	if (!token[i + 1] || token[i + 1] == ' ')
	{
		new_token = _strdup(token), free(token);
		return (token);
	}

	value = _realloc(NULL, sizeof(char) * 12);

	if (_strcmp(token + i, "$$") == 0)
		sprintf(value, "%d", getpid());
	else if (_strcmp(token + i, "$?") == 0)
		sprintf(value, "%d", shell.status);
	else
		free(value), value = _getenv(token + i + 1);

	/* Create token */
	new_token = _realloc(NULL, i + _strlen(value) + 1);
	_strncpy(new_token, token, i);
	_strcat(new_token, value);
	free(token);
	free(value);

	return (replace_vars(new_token)); /* check for more variables */
}
