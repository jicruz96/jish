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
	char *(*formatters[])(char *) = {&get_date_prompt, &get_hostname_prompt,
								    &get_shellname_prompt, &get_cwd_prompt,
								    &get_username_prompt, NULL};
	int i, j;
	char *PS1, buffer[256];
	char specifiers[] = {'d', 'H', 's', 'w', 'u', '\0'};
	static int input;

	if (fd == 2)
		write(input, "\n", 1);
	else
	{
		if (!isatty(fd))
			return;
		input = fd;
	}

	PS1 = _getenv("PS1");
	if (!PS1)
		PS1 = "$ ";

	for (i = 0; PS1[i]; i++)
		if (PS1[i] == '\\')
		{
			for (j = 0; specifiers[j]; j++)
				if (specifiers[j] == PS1[i + 1])
				{
					formatters[j](buffer);
					write(input, buffer, _strlen(buffer));
					break;
				}

			if (!specifiers[j])
				write(input, PS1 + i, 2);
			i++;
		}
		else
		{
			write(input, PS1 + i, 1);
		}
}
