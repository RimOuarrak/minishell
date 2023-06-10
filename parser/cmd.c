/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abeihaqi <abeihaqi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 10:11:32 by abeihaqi          #+#    #+#             */
/*   Updated: 2023/06/10 02:31:34 by abeihaqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	count_args(t_elem *elem)
{
	int	count;

	count = 0;
	while (elem && elem->type != PIPE_LINE)
	{
		elem = elem->next;
		count++;
	}
	return (count);
}

void	init_cmd(t_cmd *cmd, t_elem **elem)
{
	char	**tmp_args;

	tmp_args = cmd->args;
	while ((*elem))
	{
		if ((*elem) && ((*elem)->type == REDIRECTION_IN || (*elem)->type == REDIRECTION_OUT || (*elem)->type == HERE_DOC || (*elem)->type == DOUBLE_REDIRECTION_OUT))
			append_redir(cmd->redir, create_redir(elem));
		else if ((*elem))
		{
			if ((*elem)->state == GENERAL && (*elem)->type != WHITE_SPACE && (*elem)->type != QUOTE && (*elem)->type != DOUBLE_QUOTE)
			{
				while ((*elem) && (*elem)->type != WHITE_SPACE)
				{
					*tmp_args = ft_strconcat(*(tmp_args), (*elem)->content);
					(*elem) = (*elem)->next;
				}
				tmp_args++;
			}
			else if ((*elem) && (*elem)->state != GENERAL)
			{
				while ((*elem) && (*elem)->state != GENERAL)
				{
					*(tmp_args) = ft_strconcat(*(tmp_args), (*elem)->content);
					(*elem) = (*elem)->next;
				}
				tmp_args++;
			}
		}
		if ((*elem))
			(*elem) = (*elem)->next;
		if ((*elem) && (*elem)->type == PIPE_LINE && (*elem)->state == GENERAL)
			break ;
	}
}

t_cmd	*create_cmd(t_elem **elem)
{
	t_cmd	*cmd;

	cmd = ft_calloc(sizeof(t_cmd), 1);
	cmd->args = ft_calloc(sizeof(char *), count_args((*elem)) + 1);
	cmd->redir = ft_calloc(sizeof(t_redir_list), 1);
	init_cmd(cmd, elem);
	cmd->fd.in = 0;
	cmd->fd.out = 1;
	return (cmd);
}
