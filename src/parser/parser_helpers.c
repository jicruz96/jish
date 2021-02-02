#include "../shell.h"
#include <unistd.h>

/**
 * next_tok - finds next token in input buffer
 * Return: 0 on success | -1 if no next tok
 **/
int next_tok(void)
{
	if (shell.buf_i >= shell.byte_count)
		return (-1);

	while (shell.buf[shell.buf_i] == ' ' || shell.buf[shell.buf_i] == '\t')
	{
		shell.buf_i++;
		if (shell.buf_i == shell.byte_count)
			if (_getmoreline() == -1)
				return (-1);
	}

	if (shell.buf[shell.buf_i] == '#')
	{
		skip_line();
		return (-1);
	}

	if (shell.buf[shell.buf_i] == '\n')
		return (-1);

	return (0);
}

/**
 * skip_line - skips a line of user input
 **/
void skip_line(void)
{
	if (shell.buf_i >= shell.byte_count)
		return;

	while (shell.buf[shell.buf_i] != '\n')
	{
		shell.buf_i++;
		if (shell.buf_i == shell.byte_count)
			if (_getmoreline() == -1)
				return;
	}
}


/**
 * first_tok - finds start and end index of first token of user input
 * Return: start index (end index is a global variable)
 **/
int first_tok(void)
{
	int start, tmp;
	char *delims, *double_quote = "\"";
	char *all_delims =  " \t\n><&|;\"";

	if (next_tok() == -1)
		return (-1);

	start = shell.buf_i;
	if (shell.buf[shell.buf_i] == '"')
		delims = double_quote, shell.buf_i++;
	else
		delims = all_delims;

	while (true)
	{
		tmp = get_firsttok(start, delims);
		if (tmp != -2)
			return (tmp);

		if (delims == double_quote && shell.buf[shell.buf_i - 1] == '\n')
			write(STDOUT_FD, PS2, _strlen(PS2));

		if (_getmoreline() == -1)
		{
			if (replace_aliases(start))
			{
				shell.buf_i = start;
				return (first_tok());
			}
			return (start);
		}
	}

	return (0);
}

/**
 * get_firsttok - dam boii
 * @start: start index of shell.buf
 * @delims: delims that determine end of token
 * Return: -2 if nothing happened
 **/
int get_firsttok(int start, char *delims)
{
	int j;

	for ( ; shell.buf_i < shell.byte_count; shell.buf_i++)
		for (j = 0; delims[j]; j++)
			if (shell.buf[shell.buf_i] == delims[j])
			{
				if (start == shell.buf_i)
				{
					handle_syntax_error(NULL); /* is redir or sep */
					skip_line();
					return (first_tok());
				}

				if (replace_aliases(start))
				{
					shell.buf_i = start;
					return (first_tok());
				}

				return (start);
			}

	return (-2);
}

/**
 * _getmoreline - gets more line
 * Return: 0 on success | -1 on fail
 **/
int _getmoreline(void)
{
	int tmp;

	while (shell.buf_i >= shell.byte_count)
	{
		tmp = shell.byte_count + READ_SIZE + 1;
		increase_buffer(&(shell.buf), &(shell.buf_size), tmp);
		tmp = read(shell.input_fd, shell.buf + shell.byte_count, READ_SIZE);
		if (tmp <= 0)
			return (-1);
		shell.byte_count += tmp;
	}
	shell.buf[shell.byte_count] = '\0';
	return (0);
}
