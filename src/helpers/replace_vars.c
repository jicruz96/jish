#include "../shell.h"
#include <unistd.h>
#include <stdio.h>


#define END(x) (!(x) || (x) == ' ' || (x) == '.' || (x) == '/' || (x) == ',')
/**
 * replace_vars - detects and replaces vars in a shell argument
 * @arg: argument
 **/
void replace_vars(char **arg)
{
	int i, j, var_len;
	char value[512], *var, *new = NULL;
	int byte_count = _strlen(*arg);
	int size = byte_count + 1;
	int val_len = 0, buffer_shift;

	if (!byte_count)
		return;

	for (i = 0; (*arg)[i]; i++)
		if ((*arg)[i] == '$' && !END((*arg)[i + 1]))
		{
			var = *arg + i + 1;
			for (var_len = 0; !END(var[var_len]); var_len++)
				;
			val_len = _getvalue(value, var, var_len);

			/* shift arg contents */
			if (val_len > var_len + 1)
			{
				if (!new)
					new = _strdup(*arg);
				increase_buffer(&new, &size, byte_count + val_len - var_len + 1);
				buffer_shift = val_len - var_len - 1;
				for (j = byte_count - 1; j >= i + 1 + var_len; j--)
					new[j + buffer_shift] = new[j];
				*arg = new;
			}
			else
			{
				_strcpy(*arg + i + val_len, *arg + i + var_len + 1);
			}
			byte_count += val_len - var_len - 1;
			_memcpy(*arg + i, value, _strlen(value));
			i += val_len;
		}

	if (new)
		shell.open_vars[shell.open_vars_i++] = new;
}

/**
 * _getvalue - saves var value into value buffer and returns size of value
 * @value: value buffer
 * @var: variable to find value for
 * @var_len: variable length
 * Return: value length
 **/
int _getvalue(char *value, char *var, int var_len)
{
	if (_strncmp(var, "$", var_len) == 0)
		sprintf(value, "%d", getpid());
	else if (_strncmp(var, "?", var_len) == 0)
		sprintf(value, "%d", shell.status);
	else
	{
		var = _getenv(_strncpy(value, var, var_len));
		if (var)
			_strcpy(value, var);
		else
			*value = '\0';
	}

	return (_strlen(value));
}
