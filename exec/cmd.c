/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rouarrak <rouarrak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/01 23:42:59 by rouarrak          #+#    #+#             */
/*   Updated: 2023/07/03 09:16:15 by rouarrak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	handle_err(int pid)
{
	if (pid < 0)
	{
		perror("fork error");
		exit(-1);
	}
}

void	exevc(t_cmd *cmd)
{
	char	**paths;
	char	*cmd_path;

	paths = get_paths();
	cmd_path = cmd_file(paths, cmd->args[0]);
	if (!is_regular_file(cmd_path) && ft_strchr(cmd_path, '/'))
	{
		ft_putstr_fd("bash: ", 2);
		ft_putstr_fd(cmd_path, 2);
		ft_putstr_fd(": is a directory\n", 2);
		free(cmd_path);
		g_data.exit_status = 126;
		exit(g_data.exit_status);
	}
	else if (!is_regular_file(cmd_path))
		exit(g_data.exit_status);
	free_split(paths);
	execve(cmd_path, cmd->args, NULL);
	free(cmd_path);
	perror("bash");
	exit(g_data.exit_status);
}

void	exec_cmd(t_ast_node *ast_elem)
{
	signal(SIGINT, SIG_DFL);
	if (!rediring(ast_elem->content->cmd->redir->head,
			ast_elem->content->cmd))
		exit(g_data.exit_status);
	close_ast_pipe(g_data.ast.root, ast_elem->content->cmd->fd.in,
		ast_elem->content->cmd->fd.out);
	if (dup2(ast_elem->content->cmd->fd.in, STDIN_FILENO) == -1)
		perror("dup2: stdin");
	if (dup2(ast_elem->content->cmd->fd.out, STDOUT_FILENO) == -1)
		perror("dup2: stdout");
	g_data.exit_status = 0;
	if (is_builts(ast_elem->content->cmd))
		builts(ast_elem->content->cmd);
	else if (ast_elem->content->cmd->args[0])
		exevc(ast_elem->content->cmd);
	exit(g_data.exit_status);
}

void	update_status(void)
{
	int	status;

	waitpid(g_data.pid, &status, 0);
	if (WIFEXITED(status))
		g_data.exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		g_data.exit_status = WTERMSIG(status) + 128;
}

void	exec_sub(t_ast_node *ast_elem)
{
	g_data.pid = fork();
	if (g_data.pid == -1)
	{
		ft_putendl_fd("bash: fork: Resource temporarily unavailable", 2);
		return ;
	}
	if (!g_data.pid)
	{
		exec_ast(ast_elem->content->ast, ast_elem->type);
		exit(g_data.exit_status);
	}
	update_status();
}
