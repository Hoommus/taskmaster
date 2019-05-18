/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes_manipulations.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/15 15:56:52 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/02/15 15:56:52 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell_script.h"
#include "shell_script_parser.h"

t_bresult	*insert_left_recursive(t_bresult *bresult,
									t_node *parent,
									t_node *insertion)
{
	if (parent->left == NULL)
		parent->left = insertion;
	else
		insert_left_recursive(bresult, parent->left, insertion);
	return (bresult);
}
