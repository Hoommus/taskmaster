/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network_connect.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/20 19:36:29 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/05 20:00:03 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"

#define SOCKET_FILE "/var/tmp/taskmasterd.socket"

// TODO: use sysctl() to retrieve information about running taskmasterd processes
int			connect_socket_unix(void)
{
	struct sockaddr_un	*local;
	struct s_remote		*daemon;
	int					connection;

	daemon = calloc(1, sizeof(t_remote));
	local = &daemon->addr.unix;
	memset(local, 0, sizeof(struct sockaddr_un));
	local->sun_family = AF_LOCAL;
	strncpy(local->sun_path, SOCKET_FILE, sizeof(local->sun_path) - 1);
	if ((daemon->socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1)
		dprintf(2, "%s\nsocket @%s\n", strerror(errno), SOCKET_FILE);
	daemon->domain = AF_LOCAL;
	connection = connect(daemon->socket_fd, (struct sockaddr *)local, sizeof(local->sun_path));
	if (connection == -1)
	{
		dprintf(2, "%s: socket @%s\n", strerror(errno), SOCKET_FILE);
		free(daemon);
	}
	else
	{
		printf("Connection successful. Socket @%s\n", SOCKET_FILE);
		daemon->is_alive = true;
		g_shell->daemon = daemon;
	}
	return (connection);
}

int			tm_connect(const char **args)
{
	if (g_shell->daemon != NULL)
	{
		ft_printf("You are already connected to a ");
		if (g_shell->daemon->domain == AF_LOCAL)
			ft_printf("\27[1mlocal\27[0m daemon @%s\n", g_shell->daemon->addr.unix.sun_path);
		else
			ft_printf("\27[1mremote\27[0m daemon @%u\n",
				g_shell->daemon->addr.inet.sin_addr.s_addr);
		ft_printf("Execute `disconnect' command to drop any present connection "
			"(and `connect' again, if needed)\n");
		return (1);
	}
	else if (!args || !args[0])
	{
		ft_dprintf(2, "usage: connect [unix <pid> | local <pid> | inet <ip>]\n");
		return (1);
	}
	if (strcmp(args[0], "unix") == 0)
		return (connect_socket_unix());
	return (0);
}

int			tm_disconnect(const char **args)
{
	if (!args || !args[0])
	{
		ft_dprintf(2, "usage: disconnect [unix|inet ip]\n");
		return (1);
	}
	if (strcmp(args[0], "unix") == 0)
	{
		printf("Disconnected from %s\n", g_shell->daemon->addr.unix.sun_path);
		if (g_shell->daemon->connection_fd > 2)
			close(g_shell->daemon->connection_fd);
		if (g_shell->daemon->socket_fd > 2)
			close(g_shell->daemon->socket_fd);
		bzero(g_shell->daemon, sizeof(struct s_remote));
		free(g_shell->daemon);
		g_shell->daemon = NULL;
	}
	return (0);
}
