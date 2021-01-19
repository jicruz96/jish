#include "../shell.h"
#include <unistd.h>
#include <stdio.h>

/**
 * print_prompt - returns the prompt, a formatted string
 * @fd: file descriptor. determines what prompt is returned
 * Return: Prompt
 */
void print_prompt(int fd)
{
	char *(*formatters[])(void) = {&get_date_prompt, &get_hostname_prompt,
								   &get_shellname_prompt, &get_username_prompt,
								   &get_cwd_prompt, NULL};
	int i, j;
	char *prompt, *str = NULL, *tmp = NULL, *PS1 = NULL;
	char specifiers[] = {'d', 'H', 's', 'u', 'w', '\0'};
	static int input;

	if (fd == 2)
		write(input, "\n", 1);
	else
	{
		if (!isatty(fd))
			return;
		input = fd;
	}

	for (i = 0; environ[i] && !PS1; i++)
		if (_strncmp(environ[i], "PS1=", 4) == 0)
			PS1 = environ[i] + 4;
	if (!PS1)
	{
		write(fd, "$ ", 2);
		return;
	}
	prompt = _strdup(PS1);
	for (i = 0; prompt[i]; i++)
		if (prompt[i] == '\\')
			for (j = 0; specifiers[j]; j++)
				if (specifiers[j] == prompt[i + 1])
				{
					str = formatters[j]();
					tmp = _realloc(tmp, _strlen(str) + _strlen(prompt) - 1);
					sprintf(tmp, "%.*s%s%s", i, prompt, str, prompt + i + 2);
					free(prompt), free(str), prompt = _strdup(tmp), free(tmp);
					tmp = NULL;
					i -= 1;
				}
	write(input, prompt, _strlen(prompt));
	free(prompt);
}
