/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_word_types.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/09 16:35:29 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/03/02 13:25:32 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell_script.h"
#include "shell_script_parser.h"

enum e_token_type	token_class_contextual(const char *str)
{
	enum e_token_type	type;
	int					i;

	i = 0;
	type = TOKEN_NOT_APPLICABLE;
	while (str && g_tokens[i].token_name && type == TOKEN_NOT_APPLICABLE)
	{
		if (ft_strcmp(g_tokens[i].text, str) == 0)
			type = g_tokens[i].type;
		i++;
	}
	if (type == TOKEN_NOT_APPLICABLE)
	{
		type = TOKEN_WORD;
	}
	return (type);
}
