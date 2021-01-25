#ifndef SHELL_H
#define SHELL_H

#include <stdlib.h>
#include "strings/my_strings.h"

#define READ_SIZE       4096
#define HISTSIZE        4096
#define DEFAULT_LOGIC   0000
#define REDIR_OUT       0001
#define REDIR_IN        0002
#define APPEND          0004
#define HEREDOC         0010
#define AND             0020
#define OR              0040
#define PIPE            0100
#define SYNTAX_ERROR    105
#define CANT_OPEN       127

#define IS_APPEND(x) (x & APPEND)
#define IS_PIPE(x) (x & PIPE)
#define IS_AND(x) (x & AND)
#define IS_OR(x) (x & OR)
#define IS_REDIR_OUT(x) (x & REDIR_OUT)
#define IS_HEREDOC(x) (x & HEREDOC)
#define IS_NUMERIC(x) ((x) >= '0' && (x) <= '9')
#define IS_ALPHA(x) (((x) >= 'a' && (x) <= 'z') || ((x) >= 'A' && (x) <= 'Z'))
#define IS_SEPARATOR(x) (!x || *(x) == ';' || *(x) == '|' || !_strcmp(x, "&&"))
#define IS_REDIR_TOKEN(x) (*(x) == '>' || *(x) == '<')


#define true            1
#define false           0
#define STDIN_FD        0
#define STDOUT_FD       1
#define STDERR_FD       2
#define PS2             "> "

/**
 * struct command_s - command struct
 * @logic:     macro that determines logic (see macros above)
 * @command:   command as a string
 * @path:      command path as a dynamically-allocated string
 * @args:      command argument array (first arg is command)
 * @in_name:   input file name
 * @out_name:  output file name
 * @err_name:  error file name
 * @input_fd:  input fd
 * @output_fd: output_fd
 * @error_fd:  error_fd
 * @executor:  function that executes command
 **/
typedef struct command_s
{
	int logic;
	char *args[256];
	char *path;
	char *input;
	char *redirect;
	int redirect_input;
	struct command_s *next;
} command_t;

typedef int (*exec_f)(command_t *);

/**
 * struct builtin_s - builtin struct
 * @name:				builtin name
 * @function:			builtin function
 **/
typedef struct builtin_s
{
	char *name;
	exec_f function;
} builtin_t;

/**
 * struct alias_s - struct that defines an alias
 * @alias: alias name
 * @value: value of alias
 * @next: next alias value
 */
typedef struct alias_s
{
	char *alias;
	char *value;
	struct alias_s *next;
} alias_t;

/**
 * struct shell_s - shell struct
 * @name: shell name
 * @history: shell history
 * @run: if shell is running
 * @lines: lines read
 * @status: shell status
 * @history_fd: history file descriptor
 * @history_size: size of history
 * @interactive: if shell is in interactive mode
 * @aliases: alias list
 * @builtins: builtins list
 **/
typedef struct shell_s
{
	char *name;
	char **history;
	int run;
	int lines;
	int status;
	int history_fd;
	int history_size;
	int interactive;
	alias_t *aliases;
	builtin_t *builtins;
} shell_t;

/* global variable declarations */
extern char **environ;
extern shell_t shell;

/* main shell function declarations */
void shell_init(char *shellname, int input, builtin_t *builtins);
void execute_hshrc(void);
void shell_cleanup(void);

/* shell logic function declarations */
void run_shell(int fd);
void execute_line(char **tokens);
int get_IO(command_t *cmd, int prev_logic);
int get_input_fd(command_t *cmd);
int get_output_fd(command_t *cmd);
int clean_pipes(command_t *cmd);

/* Command configuration function declarations */
char **command_config(command_t *cmd, char **tokens);
char **parse_tokens(command_t *cmd, char **tokens);
int get_path(char *path_buffer, char *file);

/* execve function wrapper */
int fork_and_execute(command_t *cmd);

/* built-in function declarations */
int builtin_cd(command_t *cmd);
int builtin_alias(command_t *cmd);
int builtin_help(command_t *cmd);
int builtin_env(command_t *cmd);
int builtin_setenv(command_t *cmd);
int builtin_unsetenv(command_t *cmd);
int builtin_history(command_t *cmd);
int builtin_exit(command_t *cmd);

/* history helper function declarations */
int get_history(char *history[]);
void save_line_to_history(char *line);
void save_history_to_file(void);

/* alias helper function declarations */
char *replace_alias(char *alias);
int print_alias(char *alias);
int print_aliases(void);

/* tokenization and expansion function declarations */
int get_tokens(char **tokens, int fd);
char *tokenizer(char **string);
char *fix_dquote(char **line, char *token, int fd);
char *get_heredoc(char *end_tag, int fd);
char *replace_vars(char *token);

/* error handler function declarations */
int handle_error(command_t *cmd);
int handle_syntax_error(char *token);

/* prompt handling function declarations */
void print_prompt(int fd);
char *get_date_prompt(char *buffer);
char *get_hostname_prompt(char *buffer);
char *get_username_prompt(char *buffer);
char *get_shellname_prompt(char *buffer);
char *get_cwd_prompt(char *buffer);

/* help function declarations */
void help_help(void);
void help_alias(void);
void help_exit(void);
void help_cd(void);
void help_history(void);
void help_env(void);
void help_setenv(void);
void help_unsetenv(void);

/* other helpers function declarations */
int _unsetenv(char *key);
int _setenv(char *key, char *value);
char *_getenv(char *key);
char *_realloc(char *p, int size);

command_t *parser(int fd);
void execute_commands(command_t *cmd);
void free_command_chain(command_t *head);
exec_f get_executor(char *command);




#endif /* SHELL_H */

#ifndef GETLINE_H
#define GETLINE_H

/**
 * struct reader_s -    associates a file descriptor to its file's contents.
 *                      The i allows the struct to track what's been read.
 * @fd:			file descriptor
 * @buf:		file's text
 * @bufsize:    size of file's text buffer
 * @bytes:	    number of bytes copied into buffer (needed to differentiate
 *				between copied null bytes and null bytes from initialization)
 * @next:		pointer to next reader struct
 **/
typedef struct reader_s
{
	int fd;
	char *buf;
	int bufsize;
	int bytes;
	struct reader_s *next;
} reader_t;

/* _getline.c declarations */
char *_getline(const int fd);
char *find_line(reader_t *rd);
void _memset(char *s, int c, int n);
char *_memcpy(char *dest, char *src, int n);

#endif /* GETLINE_H */
