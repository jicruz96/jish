# -*- MakeFile -*-

CC = gcc
SRC = src/main.c src/command.c src/exec.c src/helpers/_getenv.c src/helpers/get_token.c src/helpers/handle_error.c src/helpers/increase_buffer.c src/helpers/_memcpy.c src/helpers/_memset.c src/helpers/other.c src/helpers/print_prompt.c src/helpers/prompt_helpers.c src/helpers/_realloc.c src/helpers/replace_vars.c src/strings/_puts.c src/strings/_strcat.c src/strings/_strcmp.c src/strings/_strcpy.c src/strings/_strdup.c src/strings/string_array_allocators.c src/strings/_strlen.c src/built-ins/alias.c src/built-ins/cd.c src/built-ins/env.c src/built-ins/exit.c src/built-ins/help.c src/built-ins/history.c src/built-ins/ls.c src/built-ins/setenv.c src/built-ins/unsetenv.c src/parser/parser.c src/parser/parser_helpers.c
OBJ = $(SRC:.c=.o)
NAME = hsh
RM = rm -f
CFLAGS = -Wall -Werror -Wextra -pedantic -g

all: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

fclean: oclean clean

oclean:
	$(RM) $(OBJ)

clean:
	$(RM) *~ $(NAME)

re: oclean all
