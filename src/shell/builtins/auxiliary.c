/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   auxiliary.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/07 18:34:55 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/07 18:39:57 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"

int			is_daemon_alive(void)
{
	if (g_shell->daemon.is_alive)
	{
		printf("You are already connected to a ");
		if (g_shell->daemon.domain == AF_LOCAL)
			printf("local daemon %s\n", g_shell->daemon.addr.unix.sun_path);
		else
			printf("remote daemon %u\n",
				   g_shell->daemon.addr.inet.sin_addr.s_addr);
		printf("Execute `disconnect' command to drop connection\n");
		return (1);
	}
	return (0);
}
