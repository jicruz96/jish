# -*- MakeFile -*-

CC = gcc
SRC = src/main.c src/tokenizing_functions.c src/shell_logic.c src/IO_logic.c src/helpers/replace_vars.c src/helpers/get_program_path.c src/helpers/_getenv.c src/helpers/_getline.c src/helpers/handle_error.c src/helpers/prompt_helpers.c src/helpers/print_prompt.c src/helpers/_realloc.c src/built-ins/alias.c src/built-ins/cd.c src/built-ins/env.c src/built-ins/setenv.c src/built-ins/unsetenv.c src/built-ins/exit.c src/built-ins/history.c src/built-ins/help.c src/strings/_strcat.c src/strings/_strcmp.c src/strings/_strcpy.c src/strings/_strdup.c src/strings/_strlen.c src/strings/_puts.c src/strings/string_array_allocators.c
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
