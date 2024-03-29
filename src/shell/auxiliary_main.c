/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   auxiliary_main.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/03 15:32:39 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/30 18:27:49 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"

#define NORETURN_INT _Noreturn int

int					display_normal_prompt(void)
{
	t_var	*user;
	char	host[1024];
	int		size;

	gethostname(host, 1023);
	host[ft_strchr(host, '.') - host] = 0;
	user = get_env_v(NULL, "USER");
	size = ft_printf(SHELL_PROMPT,
		user ? user->value : "$USER", host, g_shell->last_status ? 31 : 32);
	return (size);
}

NORETURN_INT		fatal(const char *const cause)
{
	dprintf(2, SH ": %s\n", cause);
	exit(1);
}
