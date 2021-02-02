#include "../shell.h"

/**
 * increase_buffer - adjust size of buffer
 * @buffer: buffer
 * @size: buffer size
 * @new_size: new buffer size (if bigger than size)
 **/
void increase_buffer(char **buffer, int *size, int new_size)
{
	if (*size < new_size)
	{
		*size = new_size;
		*buffer = _realloc(*buffer, *size);
	}
}
