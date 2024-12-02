#ifndef	MINI_SH_H

# include <stdio.h>
# include <stdlib.h>
# include <errno.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>

# include <string.h>

# define FALSE 0
# define TRUE 1

# define EOL	1
# define ARG	2
# define AMPERSAND 3

# define	PIPE	4

# define FOREGROUND 0
# define BACKGROUND 1

static char	input[512];
static char	tokens[1024];
char		*input_global;	// ptr
char		*token_global;	// tok

int			pipe_exists;
int			right_redirection_exists;
int			left_redirection_exists;

void		separate_cmd(char **command, char ***first_arg, char ***second_arg);
void		do_pipe(char **command);
void		do_right_redirection(char **cmd);
void		do_left_redirection(char **cmd);
#endif