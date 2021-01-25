#include "_a.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <fcntl.h>



/**
 * run_shell - executes a file line by line
 * @fd: file descriptor
 * Return: exit status
 **/
void run_shell(int fd)
{
	command_t *command_chain;

	while (shell.run)
	{
		print_prompt(fd);
		signal(SIGINT, print_prompt);
		command_chain = parser(fd);
		execute_commands(command_chain);
		shell.lines++;
	}
}

/**
 * parser - Parses a file and creates a command chain
 * @fd: file descriptor
 * Return: command chain
 **/
command_t *parser(int fd)
{
	command_t *command_chain = NULL, *command_chain_tail = NULL, *command_struct = NULL;
	char *start_of_line, *line, *command;
	char *separators[] = {"&&", "||", "|", NULL};
	int separator_logic[] = {AND, OR, PIPE};
	int i, j;
	int expecting_more = false;
	char *arg;
	char *end_tag;
	char *prev_token;

	start_of_line = _getline(fd);
	if (!start_of_line)
		return (NULL);
	line = start_of_line;

	/* create command chain */
	while ((command = tokenizer(&line)) || expecting_more)
	{
		/* if no command was found but we were expecting more, read input */
		if (!command)
		{
			free(start_of_line);
			start_of_line = _getline(fd);
			if (!start_of_line)
			{
				shell.status = handle_syntax_error(prev_token);
				free_command_chain(command_chain);
				return (NULL);
			}
			line = start_of_line;
			continue;
		}

		/* we're not expecting more */
		expecting_more = false;

		/* if command is an unexpected token, error out */
		if (IS_SEPARATOR(command) || IS_REDIR_TOKEN(command))
		{
			shell.status = handle_syntax_error(command);
			free_command_chain(command_chain);
			return (NULL);
		}

		/* make new command */
		command_struct = malloc(sizeof(command_t));
		command_struct->args[0] = command;
		i = 1;

		/* analyze all the remaining tokens in the alias string */
		while ((arg = tokenizer(&line)))
		{
			if (IS_SEPARATOR(arg))
			{
				/* apply separator logic */
				for (j = 0; separators[j]; j++)
					if (_strcmp(arg, separators[j]) == 0)
						command_struct->logic |= separator_logic[j];

				/* save previous command and say we're expecting more */
				prev_token = arg;
				expecting_more = true;
				break;
			}
			else if (arg[0] == '>')
			{
				if (arg[1] == '>')
					command_struct->logic |= APPEND;
				else
					command_struct->logic |= REDIR_OUT;
				free(arg);
				arg = tokenizer(&line);
				if (IS_SEPARATOR(arg) || IS_REDIR_TOKEN(arg))
				{
					shell.status = handle_syntax_error(arg);
					free(arg);
					free_command_chain(command_chain);
					return (NULL);
				}
				command_struct->redirect = arg;
				command_struct->redirect_input = STDOUT_FD;
			}
			else if (IS_NUMERIC(arg[0]) && arg[1] == '>')
			{
				command_struct->logic |= REDIR_OUT;
				command_struct->redirect_input = arg[0] - '0';
				free(arg);
				arg = tokenizer(&line);
				if (IS_SEPARATOR(arg) || IS_REDIR_TOKEN(arg))
				{
					shell.status = handle_syntax_error(arg);
					free(arg);
					free_command_chain(command_chain);
					return (NULL);
				}
				command_struct->redirect = arg;
			}
			else if (arg[0] == '<')
			{
				if (arg[1] != '<')
				{
					free(arg);
					arg = tokenizer(&line);
					if (IS_SEPARATOR(arg) || IS_REDIR_TOKEN(arg))
					{
						shell.status = handle_syntax_error(arg);
						free(arg);
						free_command_chain(command_chain);
						return (NULL);
					}
					command_struct->input = arg;
				}
				else
				{
					free(arg);
					command_struct->logic |= HEREDOC;
					end_tag = tokenizer(&line);
					if (IS_SEPARATOR(end_tag) || IS_REDIR_TOKEN(end_tag))
					{
						shell.status = handle_syntax_error(end_tag);
						free(end_tag);
						free_command_chain(command_chain);
						return (NULL);
					}
					command_struct->input = get_heredoc(end_tag, fd);
				}
			}
			else
			{
				if (arg[0] == '"')
					arg = fix_dquote(&line, arg, fd);
				command_struct->args[i++] = arg;
			}
		}

		/* add command struct to command chain... */
		if (command_chain_tail)
			command_chain_tail->next = command_struct;
		else
			command_chain = command_struct;
		
		/* save reference to end of chain */
		command_chain_tail = command_struct;
	}
	free(start_of_line);
	return (command_chain);
}


/**
 * free_command_chain - frees command chain
 * @head: head of list
 **/
void free_command_chain(command_t *head)
{
	command_t *tmp;

	while (head)
	{
		tmp = head->next;
		free(head);
		head = tmp;
	}
}

/**
 * get_executor - gets executor
 * @command: command
 * Return: pointer to executor function
 **/
exec_f get_executor(char *command)
{
	int i;

	for (i = 0; shell.builtins[i].name; i++)
		if (_strcmp(command, shell.builtins[i].name) == 0)
			return (shell.builtins[i].function);
	
	return (NULL);
}

/**
 * get_path - finds a file in the path
 * @path_buffer: buffer
 * @file: file name (char *)
 * Return: full file path || NULL if not found or no permission to execute
 **/
int get_path(char *path_buffer, char *file)
{
	char *path;
	struct stat file_info;
	int i = 0;

	if (!file)
		return (-1);

	for (i = 0; file[i]; i++)
		if (file[i] == '/')
		{
			if (stat(file, &file_info))
			{
				shell.status = ENOENT, *path_buffer = '\0';
				return (-1);
			}
			if (!(S_IXUSR & file_info.st_mode))
			{
				shell.status = EACCES, *path_buffer = '\0';
				return (-1);
			}
			_strcpy(path_buffer, file);
			return (0);
		}

	for (path = _getenv("PATH"); path && *path; path += i + (path[i] != 0))
	{
		for (i = 0; path[i] && path[i] != ':'; i++)
			;
		sprintf(path_buffer, "%.*s/%s", i, path, file);
		if (stat(path_buffer, &file_info) == 0)
		{
			if (!(S_IXUSR & file_info.st_mode))
			{
				shell.status = EACCES, *path_buffer = '\0';
				return (-1);
			}
			return (0);
		}
	}
	shell.status = ENOENT, *path_buffer = '\0';
	return (-1);
}


/**
 * execute_commands - executes commands in a command chain
 * @cmd: head of command chain
 **/
void execute_commands(command_t *cmd)
{
	int i, j, input_fd, output_fd, perms, pipefds[2];
	char path[256], *args[256], *arg, *value;
	char *command, *command_expansion;
	int input_copy = dup(STDIN_FD), output_copy;
	int prev_logic = DEFAULT_LOGIC;
	command_t *tmp_node;
	exec_f executor;

	for (i = 0; args[i]; i++)
		args[i] = NULL;

	while (cmd && shell.run)
	{
		/* Get path and executor */
		command = cmd->args[0];
		command_expansion = replace_vars(command);
		command = tokenizer(&command_expansion);
		executor = get_executor(command);
		if (executor == NULL)
		{
			if (get_path(path, command) == -1)
				executor = &handle_error;
			else
				executor = &fork_and_execute;
			cmd->path = path;
		}

		/* Make argument array */
		i = 0;
		args[i++] = command;
		for (; (arg = tokenizer(&command_expansion)); i++)
			args[i] = arg;
		for (j = 1; cmd->args[j]; j++)
		{
			value = replace_vars(cmd->args[j]);
			while ((arg = tokenizer(&value)))
				args[i++] = arg;
		}
		printf("i is %d\n", i);
		args[i] = NULL;

		printf("ARGS ARE:\n");
		for (i = 0; args[i]; i++)
			cmd->args[i] = args[i], printf("%d. %s\n", i, args[i]);

		/* Set up input */
		if (IS_HEREDOC(cmd->logic))
		{
			/* Close previous pipe if it was open */
			if (IS_PIPE(prev_logic))
				close(pipefds[1]);
			
			/* Make new pipe */
			pipe(pipefds);

			/* Write input into pipe */
			write(pipefds[0], cmd->input, _strlen(cmd->input));

			/* Close writing side of pipe */
			close(pipefds[0]);

			/* Set input_fd to reading side of pipe */
			input_fd = pipefds[1];
		}
		else if (IS_PIPE(prev_logic))
		{
			input_fd = pipefds[1];
		}
		else if (cmd->input)
		{
			cmd->input = replace_vars(cmd->input);
			input_fd = open(cmd->input, O_RDONLY);
		}
		else
		{
			input_fd = dup(STDIN_FD);
		}

		/* error out if input does not exist */
		if (input_fd == -1)
		{
			shell.status = errno;
			shell.status = handle_error(cmd);
			tmp_node = cmd;
			cmd = cmd->next;
			free(tmp_node);
			continue; /* remember to free current command */
		}

		/* Set up outputs */
		if (cmd->redirect)
		{
			perms = O_CREAT | O_WRONLY | (IS_APPEND(cmd->logic) * O_APPEND);
			cmd->redirect = replace_vars(cmd->redirect);
			output_fd = open(cmd->redirect, perms, 0644);
		}
		else if (IS_PIPE(cmd->logic))
		{
			pipe(pipefds);
			output_fd = pipefds[0];
		}
		else
		{
			output_fd = dup(STDOUT_FD);
		}

		/* error out if output doesn't exist */
		if (output_fd == -1)
		{
			shell.status = errno;
			shell.status = handle_error(cmd);
			tmp_node = cmd;
			cmd = cmd->next;
			free(tmp_node);
			continue;
		}

		/* Set up IO */
		dup2(input_fd, STDIN_FD);
		output_copy = dup(cmd->redirect_input);
		dup2(output_fd, cmd->redirect_input);

		/* Execute */
		shell.status = executor(cmd);

		/* Clean up */
		close(input_fd);
		close(output_fd);
		dup2(input_copy, STDIN_FD);
		dup2(output_copy, cmd->redirect_input);
		prev_logic = cmd->logic;
		for (i = 0; args[i]; i++)
			args[i] = NULL;

		/* Evaluate logic for next command */
		if (shell.status && IS_AND(cmd->logic))
			break;
		if (!shell.status && IS_OR(cmd->logic))
			break;
		if (IS_PIPE(cmd->logic) && IS_REDIR_OUT(cmd->logic))
			break;

		/* move on to next command */
		tmp_node = cmd;
		cmd = cmd->next;
		free(tmp_node);
	}
	for (i = 0; args[i]; i++)
		args[i] = NULL;
	free_command_chain(cmd);
}


/**
 * fork_and_execute - function wrapper for fork/execve
 * @cmd: command node
 * Return: exit status of execution
 **/
int fork_and_execute(command_t *cmd)
{
	pid_t child_pid;
	int status = 0;

	child_pid = fork();

	/* child executes */
	if (child_pid == 0)
	{
		shell.status = execve(cmd->path, cmd->args, environ);
		exit(handle_error(cmd));
	}

	/* Parent waits and returns status */
	if (child_pid == -1 || waitpid(child_pid, &status, 0) == -1)
		return (handle_error(cmd));
	else
		return (WEXITSTATUS(status));
}

/**
 * get_heredoc - gets heredoc
 * @end_tag: end tag that signals end of heredoc
 * @fd: file descriptor (to get more lines if needed)
 * Return: heredoc
 **/
char *get_heredoc(char *end_tag, int fd)
{
	char *token, *tmp;
	int token_length = 0, searching_for_end_tag = true;

	if (end_tag == NULL)
		return (NULL);

	token = malloc(sizeof(char) * 1);
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

/**
 * replace_alias - replaces aliases in a line with alias values
 * @line: line
 * Return: line with alias replaced for its value
 **/
char *replace_alias(char *line)
{
	alias_t *tmp = shell.aliases;
	char *str, *alias, *tmp_line = line, *buf;

	/* Get alias (first token of line) */
	alias = tokenizer(&tmp_line);

	/* Find alias value */
	for (tmp = shell.aliases; tmp; tmp = tmp->next)
		if (_strcmp(alias, tmp->alias) == 0)
		{
			str = replace_alias(tmp->value);
			buf = malloc(sizeof(char) * _strlen(str) + _strlen(tmp_line) + 1);
			_strcpy(buf, str);
			_strcat(buf, tmp_line);
			free(alias);
			return (buf);
		}

	/* If no alias value found, return line */
	free(alias);
	return (line);
}

/**
 * builtin_alias - builds alias and adds to alias struct
 * @cmd: command struct
 * Return: status
 */
int builtin_alias(command_t *cmd)
{
	int i = 0, j, status = 0;
	static alias_t *last;
	alias_t *tmp = shell.aliases, **connector;
	char error_msg[256], *str = "%s: %s not found\n", **args = cmd->args;

	if (args[1] == NULL)
		return (print_aliases());
	for (i = 1; args[i]; i++)
	{
		for (j = 0; args[i][j] && args[i][j] != '='; j++)
			;
		if (args[i][j] == '=' && j)
		{
			for (tmp = shell.aliases; tmp; tmp = tmp->next)
				if (_strncmp(tmp->alias, args[i], j) == 0)
					break;
			if (!tmp)
			{
				tmp = malloc(sizeof(alias_t));
				tmp->alias = _strndup(args[i], j);
				tmp->next = NULL;
				connector = last ? &last->next : &shell.aliases;
				*connector = tmp;
				last = tmp;
				tmp->value = NULL;
			}
			free(tmp->value);
			tmp->value = _strdup(args[i] + j + 1);
		}
		else if (print_alias(args[i]) == 0)
		{
			sprintf(error_msg, str, args[0], args[i]);
			write(STDERR_FILENO, error_msg, _strlen(error_msg));
			status = 1;
		}
	}
	return (status);
}

/**
 * print_alias - prints alias
 * @alias: alias
 * Return: 1 if printed | 0 if alias not found
 **/
int print_alias(char *alias)
{
	alias_t *tmp;

	for (tmp = shell.aliases; tmp; tmp = tmp->next)
		if (_strcmp(tmp->alias, alias) == 0)
		{
			printf("%s='%s'\n", tmp->alias, tmp->value);
			return (1);
		}
	return (0);
}

/**
 * print_aliases - prints shell alias list
 * Return: EXIT_SUCCESS
 **/
int print_aliases(void)
{
	alias_t *tmp = shell.aliases;

	while (tmp)
	{
		printf("%s='%s'\n", tmp->alias, tmp->value);
		tmp = tmp->next;
	}
	return (EXIT_SUCCESS);
}

/**
 * help_alias - prints alias help content
 **/
void help_alias(void)
{
	int i;
	char *lines[] = {
		"alias: alias [name[=value] ... ]",
		"\t`alias' with no arguments prints the list of aliases in the",
		"\tof aliases in the form alias NAME=VALUE on standard output.",
		"\tOtherwise, an alias is defined for each NAME whose VALUE is given.",
		"\tA trailing space in VALUE causes the next word to be checked for",
		"\talias substitution when the alias is expanded.  Alias returns",
		"\ttrue unless a NAME is given for which no alias has been defined.",
		NULL};

	for (i = 0; lines[i]; i++)
		_puts(lines[i]);
}

#define max(a,b) ((a)>(b)?(a):(b))
/**
 * _getline -   returns string leading up to newline in a file. subsequent
 *              calls return the following line, and so on.
 * @fd:	file descriptor
 * Return: string (not including newline, null-terminated)
 **/
char *_getline(const int fd)
{
	static reader_t *readers;
	reader_t *rd;
	char *buf;
	int bytes_read;
	char *line;

	if (fd == -1)
	{
		for (rd = readers; rd; rd = readers)
			readers = rd->next, free(rd->buf), free(rd);
		return (NULL);
	}

	for (rd = readers; rd; rd = rd->next)
		if (rd->fd == fd)
		{
			if (rd->bytes <= 0)
				rd->bytes = read(fd, rd->buf, READ_SIZE);
			goto END;
		}

	buf = malloc(sizeof(char) * READ_SIZE);
	bytes_read = read(fd, buf, READ_SIZE);
	if (bytes_read <= 0)
	{
		free(buf);
		return (NULL);
	}
	rd = malloc(sizeof(reader_t));
	if (rd == NULL)
		return (NULL);
	rd->fd    = fd;
	rd->buf   = buf;
    rd->bufsize = READ_SIZE;
	rd->bytes = bytes_read;
	rd->next  = readers;
	readers   = rd;
END:
	line = find_line(rd);
	line = replace_alias(line);
    rd->bufsize = max((_strlen(line) + rd->bytes), rd->bufsize);
	buf = malloc(sizeof(char) * rd->bufsize);
	_strcpy(buf, line);
	_memcpy(buf + _strlen(line), rd->buf, rd->bytes);
	rd->buf = buf;
	rd->bytes += _strlen(line);
	line = find_line(rd);
    return (line);
}

/**
 * find_line - parses buffer, finds end of line, adjusts buffer, returns line
 * @rd: pointer to reader
 * Return: pointer to line (must be freed by user)
 **/
char *find_line(reader_t *rd)
{
	int i, j, line_size = 0, bytes_copied = 0;
	char *line = NULL;


	while (rd->bytes > 0)
	{
		if (line_size < bytes_copied + rd->bytes + 1)
		{
			line_size += rd->bytes + 1;
			line = _realloc(line, sizeof(char) * line_size);
		}

		for (i = 0; i < rd->bytes; i++)
			if (rd->buf[i] == '\n')
			{
				rd->bytes -= ++i;
				_memcpy(line + bytes_copied, rd->buf, i);
				for (j = 0; i < rd->bufsize; j++, i++)
					rd->buf[j] = rd->buf[i];
				for (; j < rd->bufsize; j++)
					rd->buf[j] = '\0';
				return (line);
			}

		_memcpy(line + bytes_copied, rd->buf, rd->bytes);
		bytes_copied += rd->bytes;
		rd->bytes = read(rd->fd, rd->buf, rd->bufsize);
	}
	return (line);
}

/**
 * _memcpy - custom memcpy
 * @dest: memory area to copy to
 * @src: memory area to copy from
 * @n: number of bytes to set
 * Return: pointer to dest
 **/
char *_memcpy(char *dest, char *src, int n)
{
	int i;

	for (i = 0; i < n; i++)
		dest[i] = src[i];

	return (dest);
}

/**
 * _memset - custom memset
 * @s: pointer to area to set
 * @c: character to set memory with
 * @n: number of bytes to set
 **/
void _memset(char *s, int c, int n)
{
	int i;

	for (i = 0; i < n; i++)
		s[i] = c;
}
