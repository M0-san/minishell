#include "../include/minishell.h"

void free_syntax_tree(t_node *ast_node);

void	update_status_code(int code)
{
	if (code >= 0)
		set_var2(g_config.envp, "?", ft_itoa(code), false);
	else
		set_var2(g_config.envp, "?", ft_itoa(WEXITSTATUS(g_config.status)), false);
}

void	signal_handler_parent(int sig)
{
	if (sig == SIGQUIT && g_config.is_forked)
	{
		//update_status_code(131);
		write(2, "Quit: 3", 7);
	}
	if (sig == SIGQUIT && !g_config.is_forked)
	{
		rl_on_new_line();
		rl_redisplay();
	}
	if (sig == SIGINT && g_config.is_forked)
	{
		//update_status_code(130);
		write(1, "\n", 1);
		rl_on_new_line();
	}
	if (sig == SIGINT && !g_config.is_forked)
	{
		update_status_code(1);
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 1);
		rl_redisplay();
	}
}

int		parse_and_execute(t_lexer *lexer)
{
	t_node		*ast_node;
	t_error		*err;
	t_parser	*p;

	ast_node = NULL;
	p = new_parser(lexer);
	err = check_first_token(p);
	if (err->is_error)
		return (p_error(NULL, err->error_msg, p->peek_token->literal, 1));
	free(err);
	ast_node = parse_command(ast_node, p);
	if (ast_node == NULL)
		return (1);
	t_vector *v = fill_out_vector_with_commands(ast_node);
	// run_cmds((t_vector *) fill_out_vector_with_commands(ast_node));
	delete_free(v, &delete_cmd);
	free_syntax_tree(ast_node);
	free(p);

	// for(int i = 0; i < g_config.x_malloc_vec->size; ++i) {
	// 	if (at(g_config.x_malloc_vec, i)) {
	// 		free(remove_at(g_config.x_malloc_vec, i));
	// 	}
	// }
	return (0);
}

void	free_syntax_tree(t_node *ast_node)
{
	t_node *head;
	t_node *to_be_freed;
	head = ast_node->first_child;
	while(head)
	{
		to_be_freed = head;
		free(head->val.str);
		head = head->next_sibling;
		free(to_be_freed);
	}
}

char	*get_line(void)
{
	char	*line_read;

	line_read = readline ("g_config.prompt$ ");
	if (line_read && *line_read)
		add_history (line_read);
	return (line_read);
}

int		main(int ac, char **av, char **env)
{
	char	*line;
	t_lexer	*lexer;
	int 	code;

	ac = 0;
	av = NULL;
	g_config.is_forked = false;
	// fill_envp(env);
	//
	g_config.envp = new_vector();
	// g_config.prompt = strjoin_s(get_var(g_config.envp, "PWD"), " _$ ", false);
	signal(SIGQUIT, signal_handler_parent);
	signal(SIGINT, signal_handler_parent);
	// g_config.x_malloc_vec = new_vector();
	line = NULL;
	while (true)
	{
		line = get_line();
		if (line && line[0] == '\0')
		{
			free(line);
			continue;
		}
		if (!line)
		{
			if (isatty(0))
				write(2, "exit\n", 5);
			break ;
		}
		lexer = new_lexer(line, (int) ft_strlen(line));
		parse_and_execute(lexer);
		if (line)
			free (line);
		free(lexer);
	}
	code = atoi(get_var(g_config.envp, "?"));
	// TODO: free env
	return (code);
}
