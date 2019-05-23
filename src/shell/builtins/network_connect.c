/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network_connect.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/20 19:36:29 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/23 16:01:26 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"
#include <sys/sysctl.h>

#define SOCKET_FILE "/var/tmp/taskmasterd.socket"

// TODO: Check everything for errors
// TODO: use sysctl() to retrieve information about running taskmasterd processes
// TODO: destroy socket if failed to connect
int			connect_socket_unix(void)
{
	struct sockaddr_un	*unix;
	int					connection;

	g_shell->daemon = calloc(1, sizeof(t_remote));
	unix = &g_shell->daemon->addr.unix;
	memset(unix, 0, sizeof(struct sockaddr_un));
	unix->sun_family = AF_LOCAL;
	strncpy(unix->sun_path, SOCKET_FILE, sizeof(unix->sun_path) - 1);
	g_shell->daemon->socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (g_shell->daemon->socket_fd == -1)
		dprintf(2, "%s\nsocket @%s\n", strerror(errno), SOCKET_FILE);
	g_shell->daemon->domain = AF_LOCAL;
	connection = connect(g_shell->daemon->socket_fd, (struct sockaddr *)unix, sizeof(unix->sun_path));
	if (connection == -1)
		dprintf(2, "%s\nsocket @%s\n", strerror(errno), SOCKET_FILE);
	else
		printf("Connection successful. Socket @%s\n", SOCKET_FILE);
	g_shell->daemon->connection_fd = g_shell->daemon->socket_fd;
	return (connection);
}

int			tm_connect(const char **args)
{
	if (g_shell->daemon != NULL)
	{
		ft_printf("You are already connected to a ");
		if (g_shell->daemon->domain == AF_LOCAL)
			ft_printf("local daemon @%s\n", g_shell->daemon->addr.unix.sun_path);
		else
			ft_printf("remote daemon @%u\n",
				g_shell->daemon->addr.inet.sin_addr.s_addr);
		ft_printf("Execute `disconnect' command to drop any present connection\n");
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
		close(g_shell->daemon->connection_fd);
		close(g_shell->daemon->socket_fd);
		free(g_shell->daemon);
		g_shell->daemon = NULL;
	}
	return (0);
}
