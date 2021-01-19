#include "my_strings.h"
#include <unistd.h>

/**
 * _puts - custom puts
 * @s: string to print
 **/
void _puts(char *s)
{
	if (s == NULL)
		s = "(null)";

	write(1, s, _strlen(s));
	write(1, "\n", 1);
}
