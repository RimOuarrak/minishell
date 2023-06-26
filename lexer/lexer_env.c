/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_env.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abeihaqi <abeihaqi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/26 03:05:38 by abeihaqi          #+#    #+#             */
/*   Updated: 2023/06/26 08:37:12 by abeihaqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	lexer_expand_env_quote(t_linkedlist *list, char *value)
{
	list_add_back(list, list_new_elem(
			value,
			ft_strlen(value), WORD, list->tail->state));
	delete_from_list(&list, list->tail->prev);
}

void	add_envs(t_linkedlist *list, char **split)
{
	while (*split && *++split)
	{
		list_add_back(list, list_new_elem(" ", 1, WHITE_SPACE,
				list->tail->state));
		list_add_back(list, list_new_elem(*split,
				ft_strlen(*split), WORD, list->tail->state));
		free(*split);
	}
}

void	lexer_expand_env(t_linkedlist *list)
{
	char	**split;
	char	*value;

	split = NULL;
	value = get_env_variable(list->tail->content + 1);
	if (list->tail->state == IN_DOUBLE_QUOTE)
		lexer_expand_env_quote(list, value);
	else
	{
		split = ft_split(value, ' ');
		free(list->tail->content);
		list->tail->content = ft_strdup(*split);
		list->tail->len = ft_strlen(list->tail->content);
		list->tail->type = WORD;
		add_envs(list, split);
	}
	free(value);
	free_split(split);
}

void	lexer_env(t_linkedlist *list, char **line, int state)
{
	if (is_token(*(*line + 1)) && *(*line + 1) != WHITE_SPACE
		&& state != IN_QUOTE)
	{
		(*line)++;
		return ;
	}
	list_add_back(list, list_new_elem(*line,
			env_len((*line) + 1) + 1, ENV, state));
	*line += env_len((*line) + 1) + 1;
	if (list->tail && list->tail->state != IN_QUOTE
		&& *(list->tail->content + 1))
		lexer_expand_env(list);
}
