#include	"mini_sh.h"

void	separate_cmd(char **command, char ***first_arg, char ***second_arg)
{
	int	i;
	int	j;
	int	num_of_first_arg;
	int	num_of_second_arg;

	// pipe 앞에 있는 arg 개수
	num_of_first_arg = 0;
	for (i = 0; strcmp(command[i], "|") != 0 && strcmp(command[i], ">") != 0 && strcmp(command[i], "<") != 0; i++)
		num_of_first_arg++;
	i++;
	// pipe 뒤에 있는 arg 개수
	num_of_second_arg = 0;
	for ( ; command[i]; i++)
		num_of_second_arg++;
	// printf("num_of_first_arg: %d, num_of_second_arg: %d\n", num_of_first_arg, num_of_second_arg);	// first_arg 할당
	*first_arg = (char **)malloc((num_of_first_arg + 1) * sizeof(char *));
	if (!*first_arg) 
	{
		fprintf(stderr, "minish : malloc error\n");
		exit (-1);
	}
	for (i = 0; strcmp(command[i], "|") != 0 && strcmp(command[i], ">") != 0 && strcmp(command[i], "<") != 0; i++)
	{
		(*first_arg)[i] = strdup(command[i]);
		if (!(*first_arg)[i])
		{
			fprintf(stderr, "minish : malloc error\n");
			exit (-1);
		}
	}
	(*first_arg)[i] = NULL;
	i++;
	// second_arg 할당
	*second_arg = (char **)malloc((num_of_second_arg + 1) * sizeof(char *));
	if (!*second_arg) 
	{
		fprintf(stderr, "minish : malloc error\n");
		exit (-1);
	}
	for (j = 0; command[i]; i++, j++)
	{
		(*second_arg)[j] = strdup(command[i]);
		if (!(*second_arg)[j]) 
		{
			fprintf(stderr, "minish : malloc error\n");
			exit (-1);
		}
	}
	(*second_arg)[j] = NULL;
}

void	do_pipe(char **cmd)
{
	char	**first_arg;
	char	**second_arg;
	int		fd[2];
	int		pid2;

	separate_cmd(cmd, &first_arg, &second_arg);
	pipe(fd);
	pid2 = fork();
	if (pid2 < 0) 
	{
		fprintf(stderr, "minish : fork error\n");
		exit (-1);
	}
	else if (pid2 == 0)	// second child
	{
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
		close(fd[1]);
		execvp(second_arg[0], second_arg);
		// 정상 실행됐다면 아래는 실행 x
		fprintf(stderr, "minish : command not found\n");
		exit(127);
	}
	// only first child
	dup2(fd[1], STDOUT_FILENO);
	close(fd[0]);
	close(fd[1]);
	execvp(first_arg[0], first_arg);
	// 정상 실행됐다면 아래는 실행 x
	fprintf(stderr, "minish : command not found\n");
	exit(127);
}

void	do_right_redirection(char **cmd)
{
	char	**first_arg;
	char	**second_arg;
	int		fd;

	separate_cmd(cmd, &first_arg, &second_arg);
	fd = open(second_arg[0], O_RDWR | O_CREAT | S_IROTH, 0644);
	if (fd < 0)
	{
		fprintf(stderr, "minish : file open error\n");
		exit (-1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	execvp(first_arg[0], first_arg);
	// 정상 실행됐다면 아래는 실행 x
	fprintf(stderr, "minish : command not found\n");
	exit(127);
}

void	do_left_redirection(char **cmd)
{
	char	**first_arg;
	char	**second_arg;
	int		fd;

	separate_cmd(cmd, &first_arg, &second_arg);
	fd = open(second_arg[0], O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "minish : file open error\n");
		exit (-1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	execvp(first_arg[0], first_arg);
	// 정상 실행됐다면 아래는 실행 x
	fprintf(stderr, "minish : command not found\n");
	exit(127);
}
