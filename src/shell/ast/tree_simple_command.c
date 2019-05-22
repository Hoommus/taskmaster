/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree_simple_command.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/15 15:57:06 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/10 12:37:41 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell_script.h"
#include "shell_script_parser.h"

static char					**get_args(t_token *list, int length)
{
	char	**args;
	int		i;
	int		j;

	args = ft_memalloc(sizeof(char *) * (length + 1));
	i = -1;
	j = 0;
	while (list && ++i < length)
	{
		args[j++] = ft_strdup(list->value);
		list = list->next;
	}
	return (args);
}

t_bresult					*simple_command_build(const t_state *state,
											struct s_result *last_build)
{
	t_token				*list;
	t_bresult			*result;
	const int			size = last_build->consumed;

	result = ft_memalloc(sizeof(t_bresult));
	list = offset_list(state->list_offset, -size);
	result->root = ast_new_node(get_args(list, size), NODE_COMMAND);
	result->request = state->rule;
	return (result);
}
