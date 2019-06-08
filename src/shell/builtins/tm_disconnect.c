/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tm_disconnect.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/07 18:20:45 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/07 22:26:11 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"
#include "shell_builtins.h"

int			tm_disconnect(const char **args)
{
	if (!args || !args[0])
	{
		ft_dprintf(2, "usage: disconnect [unix|inet ip]\n");
		return (1);
	}
	else if (!strcmp(args[0], "unix") || !strcmp(args[0], "inet") || !strcmp(args[0], "local"))
	{
		printf("Disconnected from %s\n", g_shell->daemon.addr.unix.sun_path);
		if (g_shell->daemon.socket > 2)
			close(g_shell->daemon.socket);
		bzero(&(g_shell->daemon), sizeof(struct s_remote));
	}
	return (0);
}

