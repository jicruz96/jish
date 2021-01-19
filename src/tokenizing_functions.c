#include "shell.h"
#include "helpers/_getline.h"
#include <unistd.h>

/**
 * get_tokens - reads file and returns token array, split by spaces
 * @tokens: tokens array buffer
 * @fd: file descriptor of file to parse
 * Return: number of tokens or -1 if reached end of file
 **/
int get_tokens(char **tokens, int fd)
{
	char *line, *token, *buf;
	int i;

	if (fd == -1 || !tokens)
	{
		_getline(-1);
		return (0);
	}

	/* Get line from file. Save to buf */
	buf = _getline(fd);
	if (buf == NULL)
		return (-1);
	line = buf;

	/* Clear tokens array */
	for (i = 0; tokens[i]; i++)
		tokens[i] = NULL;

	/* Loop as long as tokenizer keeps returning tokens */
	for (i = 0; (token = tokenizer(&line)); i++)
		if (_strcmp(token, "<<") == 0)
			tokens[i++] = token, tokens[i] = get_heredoc(&line, fd);
		else if (*token == '"')
			tokens[i] = fix_dquote(&line, token, fd);
		else
			tokens[i] = token;

	/* no more tokens? cool. save (or free) buf and return */
	if (shell.interactive)
		save_line_to_history(buf);
	else
		free(buf);
	return (i);
}

/**
 * get_heredoc - gets heredoc
 * @line: line to parse
 * @fd: file descriptor (to get more lines if needed)
 * Return: heredoc
 **/
char *get_heredoc(char **line, int fd)
{
	char *token = malloc(sizeof(char) * 1), *end_tag = tokenizer(line), *tmp;
	int token_length = 0, searching_for_end_tag = true;

	if (end_tag == NULL)
		return (NULL);

	*token = '\0';
	while (searching_for_end_tag)
	{
		if (isatty(fd)) /* print PS2 prompt if stdin is terminal */
			write(fd, PS2, _strlen(PS2));

		/* get next line */
		tmp = _getline(fd);

		/* if next line is end tag or EOF, clean up and exit loop */
		if (_strncmp(tmp, end_tag, _strlen(end_tag)) == 0 || tmp == NULL)
		{
			searching_for_end_tag = false;
			free(end_tag);
		}
		else /* else, make space and concat tmp onto token */
		{
			token_length += _strlen(tmp);
			token = _realloc(token, token_length + 1);
			_strcat(token, tmp);
		}
		free(tmp);
	}
	return (token);
}

/**
 * fix_dquote - fix double quotes token
 * @line: line to parse
 * @token: token to fix
 * @fd: file descriptor (if more lines are needed)
 * Return: adjusted token
 **/
char *fix_dquote(char **line, char *token, int fd)
{
	int token_length = _strlen(token);
	int j, searching_for_dquote = (token[token_length - 1] != '"');
	char *tmp = NULL;

	while (searching_for_dquote)
	{
		if (shell.interactive) /* print PS2 prompt if stdin is terminal */
			write(fd, PS2, _strlen(PS2));

		/* save next line to end of buf */
		tmp = _getline(fd);
		_strcpy(*line, tmp);
		free(tmp);

		/* search for double quote / increment token_length */
		for (j = 0; (*line)[j] && searching_for_dquote; j++)
		{
			token_length++;
			searching_for_dquote = ((*line)[j] != '"');
		}

		/* make space for extra stuff and concatenate */
		token = _realloc(token, token_length + 1);
		_strncat(token, *line, j);

		/* point to end of line */
		*line += j;
	}
	tmp = _strndup(token + 1, token_length - 2);
	free(token);
	return (tmp);
}

/**
 * tokenizer - scans a line and returns the next token found
 * @line: pointer to line pointer
 * Return: next token parsed
 **/
char *tokenizer(char **line)
{
	char *token, *delim_tokens = "><&|;", *all_delims = " \t\n><&|;\"";
	int i = 0, j = 0;

	if (!line || !(*line))
		return (NULL);

	while (**line == ' ' || **line == '\t')
		(*line)++;

	if (!(**line) || **line == '#' || **line == '\n')
		return (NULL);

	for (j = 0; delim_tokens[j]; j++) /* detect a delim token */
		if (**line == delim_tokens[j])
		{
			if (**line == ';' || *(*line + 1) != delim_tokens[j])
				i = 1;
			else
				i = 2;
			break;
		}

	if (i == 0) /* if no delimiter tokens detected, find end of token */
	{
		all_delims = (**line == '"') ? "\"" : all_delims;

		for (i = 1; (*line)[i]; i++)
			for (j = 0; all_delims[j]; j++)
				if ((*line)[i] == all_delims[j])
					goto LOOP_EXIT;

LOOP_EXIT: /* adjust i value (for edge cases) */
		if (**line == '"' && (*line)[i] == '"')
			i += 1;
		else if ((*line)[1] == '>' && IS_NUMERIC(**line))
			i += 1;
	}
	token = _strndup(*line, i);
	*line += i;
	return (token);
}
