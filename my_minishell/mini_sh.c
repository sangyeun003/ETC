#include	"mini_sh.h"

int get_token(char **arg)
{
	int	type;

	*arg = token_global;
	while ((*input_global == ' ') || (*input_global == '\t')) 
		input_global++;
	// pipe & redirection check
	if (input_global[0] == '|')
		pipe_exists = TRUE;
	else if (input_global[0] == '>')
		right_redirection_exists = TRUE;
	else if (input_global[0] == '<')
		left_redirection_exists = TRUE;

	*token_global++ = *input_global;	// arg[i][j]에 하나씩 넣음

	switch (*input_global++)
	{
		case '\0' : 
			type = EOL; 
			break;
		case '&': 
			type = AMPERSAND;
			break;
		default : 
			type = ARG;
			while ((*input_global != ' ') && (*input_global != '&') && (*input_global != '\t') && (*input_global != '\0'))
				*token_global++ = *input_global++;
	}
	*token_global++ = '\0';
	// printf("*arg: %s\n", *arg);
	return(type);
}

int execute(char **cmd, int how)
{
	int	pid;

	pid = fork();
	// fork error
	if (pid < 0)
	{
		fprintf(stderr, "minish : fork error\n");
		return(-1);
	}
	// Child process
	else if (pid == 0)
	{
		// pipe 존재
		if (pipe_exists)
			do_pipe(cmd);
		else if (right_redirection_exists)
			do_right_redirection(cmd);
		else if (left_redirection_exists)
			do_left_redirection(cmd);
		else
		{
			execvp(cmd[0], cmd);	// *comm == comm[0](ls 같은 거)
			// 정상 실행됐다면 아래는 실행 x
			fprintf(stderr, "minish : command not found\n");
			exit(127);
		}
	}
	// Only Parent Process
	/* Background execution */
	if (how == BACKGROUND)
	{
		printf("[%d]\n", pid);
		return 0;
	}		
	/* Foreground execution */
	while (waitpid(pid, NULL, 0) < 0)	// 자식 process 기다림
		if (errno != EINTR)
			return -1;
	return 0;
}

int parse_and_execute(char *input)
{
	char	*arg[1024];
	int		type;
	int		how;
	int		quit = FALSE;
	int		n_arg = 0;
	int		finished = FALSE;

	input_global = input;	// 들어온 한 줄
	token_global = tokens;	// global static char token[1024];
	while (!finished)
	{
		switch (type = get_token(&arg[n_arg]))	// arg[]에 input에서 하나 똑 떼어서 저장
		{
			case ARG :
				n_arg++;
				break;
			case EOL :
			case AMPERSAND:
				if (!strcmp(arg[0], "quit"))
					quit = TRUE;
				else if (!strcmp(arg[0], "exit"))
					quit = TRUE;
				else if (!strcmp(arg[0], "cd")) 
				{
					/* Do something */
					if (strcmp(arg[1], "~") == 0)
					{
						char	*home_path;

						home_path = getenv("HOME");
						if (!home_path)
						{
							printf("HOME is not exist");
							return (quit);
						}
						chdir(home_path);
					}
					else
						chdir(arg[1]);
				}
				else if (!strcmp(arg[0], "type")) 
				{
					if (n_arg > 1) 
					{
						int	i, fid;
						int	readcount;
						char	buf[512];
						// 학생들이 프로그램 작성할 것
						// fid = open(arg[1], O_RDONLY);
						// if (fid >= 0) {
						// 	readcount = read(fid, buf, 512);
						// 	while (readcount > 0) {
						// 		for (i = 0; i < readcount; i++)
						// 			putchar(buf[i]);
						// 		readcount = read(fid, buf, 512);
						// 	}
						// }
						// close(fid);
					}
				}
				else 
				{
					how = (type == AMPERSAND) ? BACKGROUND : FOREGROUND;
					arg[n_arg] = NULL;
					if (n_arg != 0)
						execute(arg, how);
				}
				n_arg = 0;
				if (type == EOL)
					finished = TRUE;
				break; 
		}
	}
	return quit;
}

int	main()
{
    // char	*arg[1024];
	int		quit;

	printf("msh # ");
	while (gets(input))		// char input[512]: 명령어 받아옴
	{
		quit = parse_and_execute(input);	// parsing & execute
		if (quit)
			break;
		// 다음 cmd를 위해 flag 초기화
		pipe_exists = 0;
		right_redirection_exists = 0;
		left_redirection_exists = 0;
		printf("msh # ");
	}
	return (0);
}
