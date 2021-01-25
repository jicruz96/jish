#include "_a.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

shell_t shell = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int main(void)
{
    alias_t *tmp;
    char *line;
    int fd;

    tmp = malloc(sizeof(alias_t));
    tmp->alias = "this";
    tmp->value = "the value is here";
    tmp->next = malloc(sizeof(alias_t));
    tmp->next->alias = "that";
    tmp->next->value = "this and the rest is that value";
    tmp->next->next = malloc(sizeof(alias_t));
    tmp->next->next->alias = "a";
    tmp->next->next->value = "that and this is a stuff";
    tmp->next->next->next = malloc(sizeof(alias_t));
    tmp->next->next->next->alias = "b";
    tmp->next->next->next->value = "this is\na MULTILINE alias\nokay?";
    tmp->next->next->next->next = NULL;
    shell.aliases = tmp;

    fd = open("file", O_RDONLY);
    while ((line = _getline(fd)))
        printf("%s", line);
    return (0);
}
