#include "../shell.h"
#include <stdio.h>
#include <unistd.h>


#define END(x) (!(x) || (x) == ' ' || (x) == '.' || (x) == '/' || (x) == ',')
/**
 * replace_vars - detects an replaces variables in a shell token
 * @token: token
 * Return: token with all variables replaces
 **/
char *replace_vars(char *token)
{
	int i, j, k, l;
	char buffer[256], value[256], *variable;

	if (!token)
		return (NULL);

	for (i = 0; i < 256; i++)
		buffer[i] = 0, value[i] = 0;

	for (i = 0, j = 0; token[i]; i++)
		if (token[i] == '$' && !END(token[i + 1]))
		{
			variable = token + i + 1;
			for (k = 0; !END(variable[k]); k++)
				;

			if (_strncmp(variable, "$", k) == 0)
				sprintf(value, "%d", getpid());
			else if (_strncmp(variable, "?", k) == 0)
				sprintf(value, "%d", shell.status);
			else
			{
				variable = _getenv(_strncpy(value, variable, k));
				if (variable)
					_strcpy(value, variable);
				else
					*value = '\0';
			}

			for (l = 0; value[l]; l++)
				buffer[j++] = value[l], value[l] = '\0';
			i += k;
		}
		else
		{
			buffer[j++] = token[i];
		}
	buffer[j] = '\0';
	free(token);
	return (_strdup(buffer));
}
