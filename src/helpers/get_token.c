#include "../shell.h"
#include <unistd.h>

/**
 * get_token - copies a token to a buffer
 * @buffer: buffer
 **/
void get_token(char **buffer)
{
	char *delims, *double_quote = "\"";
	char *all_delims = " \t\n><&|;\"";
	int j;

	delims = (shell.buf[shell.buf_i] == '"') ? double_quote : all_delims;
	*buffer = shell.line + shell.line_i;
	shell.line[shell.line_i++] = shell.buf[shell.buf_i++];
	while (true)
	{
		while (shell.buf_i < shell.byte_count)
		{
			for (j = 0; delims[j]; j++)
				if (shell.buf[shell.buf_i] == delims[j])
				{
					if (delims == double_quote)
						shell.line[shell.line_i++] = shell.buf[shell.buf_i++];
					shell.line[shell.line_i++] = '\0';
					return;
				}

			shell.line[shell.line_i++] = shell.buf[shell.buf_i++];
		}

		if (delims == double_quote && shell.interactive)
			write(STDOUT_FD, PS2, _strlen(PS2));
		if (_getmoreline() == -1)
		{
			shell.line[shell.line_i++] = '\0';
			return;
		}
	}
}
