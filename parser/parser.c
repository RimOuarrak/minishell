/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abeihaqi <abeihaqi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 06:58:02 by abeihaqi          #+#    #+#             */
/*   Updated: 2023/06/26 02:52:51 by abeihaqi         ###   ########.fr       */
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

t_ast_node	*ft_parser(t_elem **elem, t_ast_node *ast)
{
	t_ast_node	*ast_new;

	ast_new = NULL;
	if ((*elem) && !is_type_state((*elem), PARENTASIS_CLOSE, GENERAL))
	{
		(*elem) = (*elem)->next;
		return (ast);
	}
	if (!*elem)
		return (ast);
	ast_new = ft_calloc(sizeof(t_ast_node), 1);
	while ((*elem) && (*elem)->type == WHITE_SPACE && (*elem)->next)
		(*elem) = (*elem)->next;
	if ((*elem) && !is_logical_operator((*elem)) && (*elem)->type != PARENTASIS_OPEN)
	{
		return (create_cmd(elem));
	}
	if ((*elem) && is_logical_operator((*elem)) && (*elem)->state == GENERAL && ast && ast)
	{
		ast_new->type = node_type((*elem));
		(*elem) = (*elem)->next;
		ast_new->content = ft_calloc(sizeof(t_union), 1);
		ast_new->content->pipe = ft_calloc(sizeof(t_pipe), 1);
		ast_new->content->pipe->first = ast;
		if (*elem)
			ast_new->content->pipe->second = ft_parser(elem, ast_new->content->pipe->second);
	}
	if (*elem && !is_type_state((*elem), PARENTASIS_OPEN, GENERAL))
	{
		(*elem) = (*elem)->next;
		ast_new->type = SUB;
		ast_new->content = ft_calloc(sizeof(t_union), 1);
		while (*elem && (*elem)->type != PARENTASIS_CLOSE)
			ast_new->content->ast = ft_parser(elem, ast_new->content->ast);
	}
	return (ast_new);
}
