/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes_memory.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/15 15:56:56 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/05 16:21:32 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell_script.h"
#include "shell_script_parser.h"

t_node	*ast_new_node(void *value, enum e_node_type node_type)
{
	t_node	*node;

	node = ft_memalloc(sizeof(t_node));
	node->node_type = node_type;
	node->command_args = value;
	return (node);
}

void	free_arr(void **arr)
{
	int		i;

	i = 0;
	while (arr && arr[i])
		ft_memdel(&(arr[i++]));
	ft_memdel(arr);
}


void	ast_free_recursive(t_node *node)
{
	if (node->left != NULL)
		ast_free_recursive(node->left);
	if (node->right != NULL)
		ast_free_recursive(node->right);
	if (node->node_type == NODE_COMMAND)
	{
		free_arr((void **)node->command_args);
		ft_memdel((void **)&(node->command_args));
	}
	ft_memdel((void **)&node);
}
