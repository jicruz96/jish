#include "shell.h"

/**
 * command_init - returns new instance of command_t object
 * @command: command (string) to include in first index of cmd->args
 * Return: pointer to new instance of command_t object
 **/
command_t *command_init(char *command)
{
	command_t *cmd = malloc(sizeof(command_t));

	cmd->logic = DEFAULT_LOGIC;
	cmd->args[0] = command;
	cmd->input = NULL;
	cmd->redirect = NULL;
	cmd->redirect_input = STDOUT_FD;
	cmd->input_fd = STDIN_FD;
	cmd->output_fd = STDOUT_FD;
	cmd->next = NULL;
	return (cmd);
}

/**
 * command_config - configures a command object
 * @cmd: command object
 * Return: 0 if end of newline reached | -1 if error | num chars else
 **/
int command_config(command_t *cmd)
{
	char *separators[] = {"&&", "||", "|", NULL};
	int separator_logic[] = {AND, OR, PIPE};
	int j, sep_len, args_index = 1;

	while (next_tok() != -1)
	{
		for (j = 0; separators[j]; j++)
		{
			sep_len = _strlen(separators[j]);
			if (_strncmp(shell.buf + shell.buf_i, separators[j], sep_len) == 0)
			{
				cmd->logic |= separator_logic[j];
				return (shell.buf_i);
			}
		}

		if (IS_REDIR_IN_TOKEN(shell.buf + shell.buf_i))
		{
			cmd->logic |= (HEREDOC * (shell.buf[++shell.buf_i] == '<'));
			shell.buf_i += (shell.buf[shell.buf_i] == '<');
			if (set_redir(cmd->input) == -1)
			{
				skip_line();
				return (-1);
			}
		}
		else if (IS_REDIR_OUT_TOKEN(shell.buf + shell.buf_i))
		{
			if (set_redir_out(cmd) == -1)
				return (-1);
		}
		else /* otherwise, it's a token. add to cmd->args array */
		{
			get_token(&(cmd->args[args_index++]));
			if (shell.buf_i >= shell.byte_count)
				break;
		}
	}
	cmd->args[args_index] = NULL;
	return (0);
}

/**
 * set_redir_out - sets redirection logic for a command object and finds
 *                 redirection file name
 * @cmd:           command object
 * Return: 0 if successful | -1 if failure
 **/
int set_redir_out(command_t *cmd)
{
	if (IS_APPEND_TOKEN(shell.buf + shell.buf_i))
	{
		cmd->logic |= APPEND;
		cmd->redirect_input = STDOUT_FD;
		shell.buf_i += 2;
	}
	else
	{
		cmd->logic |= REDIR_OUT;
		if (IS_NUMERIC(shell.buf[shell.buf_i]))
		{
			cmd->redirect_input = shell.buf[shell.buf_i] - '0';
			shell.buf_i += 2;
		}
		else
		{
			cmd->redirect_input = STDOUT_FD;
			shell.buf_i += 1;
		}
	}

	if (set_redir(cmd->redirect) == -1)
	{
		skip_line();
		return (-1);
	}

	return (0);
}


/**
 * set_redir - copies a token into a buffer that belongs to either cmd->input
 *             or cmd->redirect
 * @buffer: either cmd->input or cmd->redirect
 * Return: 0 on success | -1 on failure
 **/
int set_redir(char *buffer)
{
	/* if there is no next token, error out */
	if (next_tok() == -1)
	{
		handle_syntax_error(NULL);
		return (-1);
	}

	/* if the next token is an invlaid token, error out */
	if (IS_SEPARATOR(shell.buf + shell.buf_i))
	{
		handle_syntax_error(NULL); /* shell.buf */
		skip_line();
		return (-1);
	}

	if (IS_REDIR_TOKEN(shell.buf + shell.buf_i))
	{
		handle_syntax_error(NULL); /* shell.buf */
		skip_line();
		return (-1);
	}

	get_token(&buffer);
	return (0);
}

/**
 * free_command_chain - frees command chain
 * @head: head of list
 **/
void free_command_chain(command_t **head)
{
	command_t *tmp = *head;

	while (*head)
	{
		tmp = (*head)->next;
		free(*head);
		*head = tmp;
	}
}
