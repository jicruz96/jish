#include "../shell.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

/**
 * parser - returns string leading up to newline in a file. subsequent
 *          calls return the following line, and so on.
 * Return:  string (not including newline, null-terminated)
 **/
command_t *parser(void)
{
	int start, read_return = 1;
	command_t *head = NULL, *cmd = NULL;
	char *command;

	if (parser_init() == -1)
		return (NULL);

	for (; read_return > 0 && _getmoreline() != -1; shell.buf_i++)
	{
START:
		start = first_tok();
		if (start == -1)
		{
			if (cmd && EXPECTING_MORE(cmd->logic))
			{
				if (shell.buf_i < shell.byte_count)
				{
					if (shell.interactive)
						write(shell.input_fd, PS2, _strlen(PS2));
					if (_getmoreline() != -1)
						goto START;
				}
				handle_syntax_error(NULL); /* expecting more */
				free_command_chain(&head);
			}
			break;
		}
		command = shell.line + shell.line_i;
		_strncpy(command, shell.buf, shell.buf_i - start);
		if (cmd)
			cmd->next = command_init(command), cmd = cmd->next;
		else
			head = command_init(command), cmd = head;
		shell.line_i += (shell.buf_i - start) + 1;

		read_return = command_config(cmd);    /* command config */
		if (read_return == -1)
			free_command_chain(&head);
	}
	return (parser_cleanup(head));
}

/**
 * parser_cleanup - resets buffers/globals for next parser call
 * @head: head of list to return
 * Return: head
 **/
command_t *parser_cleanup(command_t *head)
{
	shell.byte_count -= ++shell.buf_i;
	if (shell.byte_count < 0)
		shell.byte_count = 0;
	_memcpy(shell.buf, shell.buf + shell.buf_i, shell.byte_count);
	shell.buf_i = 0;
	shell.line_i = 0;
	return (head);
}

/**
 * parser_init - reads line and null-terminates buffer
 * Return: 0 on success | -1 on failure or EOF
 **/
int parser_init(void)
{
	if (shell.byte_count <= 0)
		shell.byte_count = read(shell.input_fd, shell.buf, READ_SIZE);

	if (shell.byte_count <= 0)
		return (-1);

	shell.buf[shell.byte_count] = '\0';
	return (0);
}
