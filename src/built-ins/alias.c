#include "../shell.h"
#include <unistd.h>
#include <stdio.h>

/**
 * get_alias - returns alias of a token (or token itself if no alias)
 * @alias: alias
 * Return: alias value
 **/
char *get_alias(char *alias)
{
	alias_t *tmp = shell.aliases;
	char *str;

	if (shell.aliases)
		for (; tmp; tmp = tmp->next)
			if (_strcmp(alias, tmp->alias) == 0)
			{
				free(alias);
				str = get_alias(_strdup(tmp->value));
				return (str);
			}

	str = _strdup(alias);
	free(alias);
	return (str);
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
