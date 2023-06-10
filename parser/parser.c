/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abeihaqi <abeihaqi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 06:58:02 by abeihaqi          #+#    #+#             */
/*   Updated: 2023/06/10 04:05:45 by abeihaqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

enum e_node_type	node_type(t_elem *elem)
{
	if (elem->type == DOUBLE_AMPERSAND)
		return (AND);
	if (elem->type == LOGICAL_OR_OPERATOR)
		return (OR);
	if (elem->type == PIPE_LINE)
		return (PIPE);
	return (CMD);
}

void	ft_parser(t_elem *elem, t_ast_node **ast)
{
	t_ast_node	*tmp_ast_node;

	while (elem->type == WHITE_SPACE && elem->next)
		elem = elem->next;
	if (elem && (elem->type == WORD || is_redirection(elem)))
	{
		if (!(*ast))
		{
			(*ast) = ft_calloc(sizeof(t_ast_node), 1);
		}
		(*ast)->type = node_type(elem);
		(*ast)->content = ft_calloc(sizeof(t_union), 1);
		(*ast)->content->cmd = create_cmd(&elem);
	}
	if (elem && is_logical_operator(elem) && elem->state == GENERAL && ast && *ast && elem->next)
	{
		tmp_ast_node = ft_calloc(sizeof(t_ast_node), 1);
		tmp_ast_node->type = node_type(elem);
		tmp_ast_node->content = ft_calloc(sizeof(t_union), 1);
		tmp_ast_node->content->pipe = ft_calloc(sizeof(t_pipe), 1);
		tmp_ast_node->content->pipe->first = *ast;
		*ast = tmp_ast_node;
		(*ast)->content->pipe->second = ft_calloc(sizeof(t_ast_node), 1);
		ft_parser(elem->next, &(*ast)->content->pipe->second);
	}
}
