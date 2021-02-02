#ifndef SHELL_H
#define SHELL_H

#include <stdlib.h>
#include "strings/my_strings.h"

#define READ_SIZE       1
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
#define IS_REDIR_OUT_TOKEN(x) (*(x) == '>' ||\
								(IS_NUMERIC(*(x)) && (x)[1] == '>'))
#define IS_REDIR_IN_TOKEN(x) (*(x) == '<')
#define IS_APPEND_TOKEN(x) ((x)[0] == '>' && (x)[1] == '>')
#define IS_REDIR_TOKEN(x) (IS_REDIR_OUT_TOKEN(x) || IS_REDIR_IN_TOKEN(x))
#define EXPECTING_MORE(x) (IS_AND(x) || IS_OR(x) || IS_PIPE(x))



#define true            1
#define false           0
#define STDIN_FD        0
#define STDOUT_FD       1
#define STDERR_FD       2
#define PS2             "> "

/**
 * struct command_s - command struct
 * @logic:          macro that determines logic (see macros above)
 * @args:           command argument array (first arg is command)
 * @input:          command input (string)
 * @redirect:       command redirection destination
 * @redirect_input: command redirection source
 * @input_fd:       input fd
 * @output_fd:      output_fd
 * @next:           pointer to next command struct
 **/
typedef struct command_s
{
	int logic;
	char *args[256];
	char *input;
	char *redirect;
	int redirect_input;
	int input_fd;
	int output_fd;
	struct command_s *next;
} command_t;

typedef int (*exec_f)(char *args[]);

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
 * @size: size of alias
 * @next: next alias value
 */
typedef struct alias_s
{
	char *alias;
	char *value;
	int size;
	struct alias_s *next;
} alias_t;

/**
 * struct shell_s - shell struct
 * @name: shell name
 * @input_fd: input file descriptor
 * @lines: lines read
 * @line: helper buffer
 * @buf: user input buffer
 * @open_vars: list of variable strings
 * @buf_size: size of buf
 * @line_i: current reading index of shell.line
 * @buf_i: current reading index of shell.buf
 * @open_vars_i: current reading index of shell.open_vars
 * @byte_count: number of bytes in shell.buf
 * @run: if shell is running
 * @status: shell status
 * @history_size: size of history
 * @interactive: if shell is in interactive mode
 * @history: shell history
 * @aliases: alias list
 * @builtins: builtins list
 **/
typedef struct shell_s
{
	char *name;
	int input_fd;
	int lines;
	char line[4096];
	char *buf;
	char *open_vars[10];
	int buf_size;
	int line_i;
	int buf_i;
	int open_vars_i;
	int byte_count;
	int run;
	int status;
	int history_size;
	int interactive;
	char **history;
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
char **parse_tokens(command_t *cmd, char **tokens);
int get_path(char *path_buffer, char *file);

/* execve function wrapper */
int fork_and_execute(char *args[]);

/* built-in function declarations */
int builtin_cd(char *args[]);
int builtin_alias(char *args[]);
int builtin_help(char *args[]);
int builtin_env(char *args[]);
int builtin_setenv(char *args[]);
int builtin_unsetenv(char *args[]);
int builtin_history(char *args[]);
int builtin_exit(char *args[]);
int builtin_ls(char *args[]);

/* history helper function declarations */
int get_history(char *history[]);
void save_line_to_history(char *line);
void save_history_to_file(void);

/* alias helper function declarations */
int get_alias(char **alias_buffer, int *buffer_size, char *alias);
int print_alias(char *alias);
int print_aliases(void);

/* tokenization and expansion function declarations */
void fix_dquote(char *arg);
char *get_heredoc(char *end_tag, int fd);
void replace_vars(char **arg);

/* error handler function declarations */
int handle_error(char *args[]);
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

command_t *parser(void);
void execute_commands(command_t *cmd);
void free_command_chain(command_t **head);
exec_f get_executor(char *command);
command_t *command_init(char *command);

void setup_args(char *args[]);
int replace_alias(int *i, int *bytes);
int replace_aliases(int start);
void increase_buffer(char **buffer, int *size, int new_size);
int command_config(command_t *cmd);
int next_tok(void);
void get_token(char **buffer);
int set_redir(char *buffer);
void skip_line(void);
int first_tok(void);
int _getmoreline(void);
void _memset(char *s, int c, int n);
char *_memcpy(char *dest, char *src, int n);
int get_input(command_t *cmd, int *pipefds[2], int prev_logic);
int get_output(command_t *cmd, int **pipefds);
int parser_init(void);
command_t *parser_cleanup(command_t *head);
int set_redir_out(command_t *cmd);
int get_firsttok(int start, char *delims);
int _getvalue(char *value, char *var, int var_len);







#endif /* SHELL_H */
