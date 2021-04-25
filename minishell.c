#include "minishell.h"

#define IS_FIRST 0
#define IS_LAST 2
#define IS_MIDDLE 1
#define IS_FIRSTLAST 3

int	fill_envp(char **envp)
{
	int		i;

	i = -1;
	if (!envp)
		return (1);
	g_envp = new_vector();
	while (envp[++i] != NULL)
	{
		g_envp->insert(g_envp, strdup(envp[i]));
	}
	return (0);
}

char *to_string(void *item)
{
	char *it = (char *)item;
	return (strcat(it, "\n"));
}

int get_position(t_size size, int index)
{
	if (size == 1)
		return (IS_FIRSTLAST);
	if (index == 0)
		return (IS_FIRST);
	if (index == size - 1)
		return (IS_LAST);
	return (IS_MIDDLE);
}

void setup_redirection(t_type type, char *arg)
{
	int flags;
	int fd;

	if (type == right)
	{
		fd = open(arg, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		dprintf(2, "OPEN: FNAME(%s) AS [WRITE] FD(%d)\n", arg, fd);
	}
	else if (type == right_append)
	{
		fd = open(arg, O_CREAT | O_APPEND | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		dprintf(2, "OPEN: FNAME(%s) AS [WRITE/APPEND] FD(%d)\n", arg, fd);
	}
	else if (type == left)
	{
		fd = open(arg, O_RDONLY);
		dprintf(2, "OPEN: FNAME(%s) AS [READ] FD(%d)\n", arg, fd);
	}
	if (fd < 0)
	{
		perror(arg);
		exit(1);
	}
	if (type == right || type == right_append)
		dup2(fd, 1);
	else if (type == left)
		dup2(fd, 0);
}

void close_redirs()
{
}

void setup_all_redirs(t_vector *redirs)
{
	int i;
	t_redir *redir;

	i = -1;
	while (++i < redirs->size)
	{
		redir = (t_redir *)redirs->at(redirs, i);
		setup_redirection(redir->type, redir->arg);
		free(redir->arg);
	}
}

void setup_pipes(int fd[][2], int position, int index)
{
	const int prev_idx = index - 1;

	if (position == IS_FIRST)
	{
		dprintf(2, "CHILD: DUP(1) INDEX(%d)\n", index);
		dup2(fd[index][1], 1);
		close(fd[index][0]);
	}
	else if (position == IS_MIDDLE)
	{
		dprintf(2, "CHILD: DUP(ALL) INDEX(%d, %d)\n", prev_idx, index);
		dup2(fd[prev_idx][0], 0);
		dup2(fd[index][1], 1);
		close(fd[prev_idx][1]);
	}
	else if (position == IS_LAST)
	{
		dprintf(2, "CHILD: DUP(0) INDEX(%d)\n", prev_idx);
		dup2(fd[prev_idx][0], 0);
		close(fd[index][1]);
		close(fd[index][0]);
		close(fd[prev_idx][1]);
	}
	else
	{
		dprintf(2, "CHILD: DUP(NONE)\n");
	}
}

void close_pipes(int fd[][2], int pos, int index)
{
	if (pos == IS_FIRST)
		close(fd[index][1]);
	if (pos == IS_MIDDLE)
	{
		close(fd[index - 1][0]);
		close(fd[index][1]);
	}
	if (pos == IS_LAST)
		close(fd[index - 1][0]);
}

pid_t run_command(t_cmd *cmd, int fd[][2], t_size size, int index)
{
	pid_t pid;
	int pos;

	if ((pid = fork()) < 0)
		ft_exit("Error\nFORK FAILED!", -1);
	pos = get_position(size, index);
	if (pid == 0)
	{
		setup_pipes(fd, pos, index);
		if (cmd->redirs != NULL && !is_empty(cmd->redirs))
			setup_all_redirs(cmd->redirs);
		dprintf(2, "CHILD: EXEC(%s)\n", cmd->argv[0]);
		execvp(cmd->argv[0], cmd->argv);
		ft_exit("Error\nEXECVE FAILED!", -1);
	}
	close_pipes(fd, pos, index);
	return pid;
}

/*
** HELPER FUNC
*/
t_redir *create_redir(t_type type, char *arg)
{
	t_redir *r = malloc(sizeof(t_redir));
	r->arg = strdup(arg);
	r->type = type;
	return (r);
}

t_cmd *create_cmd(char *arg1, char *arg2, char *arg3, char *arg4, char *arg5)
{
	t_cmd *cmd;

	cmd = (t_cmd *)malloc(sizeof(t_cmd));
	cmd->argv[0] = arg1;
	cmd->argv[1] = arg2;
	cmd->argv[2] = arg3;
	cmd->argv[3] = arg4;
	cmd->argv[4] = arg5;
	cmd->argv[5] = NULL;
	cmd->redirs = new_vector();
	return (cmd);
}

t_vector *fill_commands()
{
	t_vector *cmds = new_vector();

	t_cmd *cm1 = create_cmd("echo", "14 + 19", NULL, NULL, NULL);
	cm1->redirs->insert(cm1->redirs, create_redir(right, "a"));
	cm1->redirs->insert(cm1->redirs, create_redir(right, "b"));
	cm1->redirs->insert(cm1->redirs, create_redir(right, "c"));

	t_cmd *cm2 = create_cmd("bc", NULL, NULL, NULL, NULL);
	cm2->redirs->insert(cm2->redirs, create_redir(left, "a"));
	cm2->redirs->insert(cm2->redirs, create_redir(left, "b"));
	cm2->redirs->insert(cm2->redirs, create_redir(left, "c"));
	cm2->redirs->insert(cm2->redirs, create_redir(right, "e"));
	cm2->redirs->insert(cm2->redirs, create_redir(right, "f"));

	t_cmd *cm3 = create_cmd("cat", "f", NULL, NULL, NULL);

	// t_cmd *cm4 = create_cmd("BC", NULL, NULL, NULL, NULL);

	// t_cmd *cm5 = create_cmd("echo", "a", "+", "b", NULL);

	// t_cmd *cm6 = create_cmd("cat", NULL, NULL, NULL, NULL);
	// cm6->redirs->insert(cm6->redirs, create_redir(right, "file"));

	// t_cmd *cm7 = create_cmd("cat", NULL, NULL, NULL, NULL);
	// cm7->redirs->insert(cm7->redirs, create_redir(left, "file"));
	// cm7->redirs->insert(cm7->redirs, create_redir(right, "file"));

	///////////
	cmds->insert(cmds, cm1);
	cmds->insert(cmds, cm2);
	cmds->insert(cmds, cm3);
	// cmds->insert(cmds, cm4);
	// cmds->insert(cmds, cm5);
	// cmds->insert(cmds, cm6);
	// cmds->insert(cmds, cm7);

	return (cmds);
}

int main(int ac, char **av, char **env)
{
	int i = -1;
	int j = -1;
	int fd[100][2];
	pid_t pids[64];

	fill_envp(env);

	display_vector(g_envp, to_string);

	exit(0);

	t_vector *cmds = fill_commands();

	while (++i < cmds->size)
	{
		pipe(fd[i]);
		printf("PARENT:LOOP INDX: %d\n", i);
		t_cmd *cmd = (t_cmd *)cmds->at(cmds, i);
		pids[i] = run_command(cmd, fd, cmds->size, i);
	}
	i = -1;
	while (++i < cmds->size)
		if (pids[i] > 0)
			wait(&pids[i]);

	return (0);
}