#ifndef GETLINE_H
#define GETLINE_H

/**
 * struct reader_s -    associates a file descriptor to its file's contents.
 *                      The i allows the struct to track what's been read.
 * @fd:			file descriptor
 * @buf:		file's text
 * @bytes:	number of bytes copied into buffer (needed to differentiate
 *				between copied null bytes and null bytes from initialization)
 * @next:		pointer to next reader struct
 **/
typedef struct reader_s
{
	int fd;
	char *buf;
	int bytes;
	struct reader_s *next;
} reader_t;

/* _getline.c declarations */
char *_getline(const int fd);
char *find_line(reader_t *rd);
void _memset(char *s, int c, int n);
char *_memcpy(char *dest, char *src, int n);

#endif /* GETLINE_H */
