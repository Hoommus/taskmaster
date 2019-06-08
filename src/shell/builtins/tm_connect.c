/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tm_connect.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/20 19:36:29 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/08 21:08:01 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <taskmaster_cli.h>
#include "taskmaster_cli.h"

#define SOCKET_FILE "/var/tmp/taskmasterd.socket"
#define MAX_RETRIES 5

static struct timeval	g_to = {1, 0};
static int				g_ms = 1000;
static struct s_packet	*g_handshake;

int			check_connection(int socket)
{
	struct pollfd	pfd;
	int				retries;
	int				s;
	int				flags;

	flags = fcntl(socket, F_GETFL);
	retries = 0;
	pfd = (struct pollfd){socket, POLLIN | POLLOUT, 0};
	while (retries < MAX_RETRIES && (s = poll(&pfd, 1, g_ms << retries)) == 0)
		if (retries + 1 == MAX_RETRIES)
		{
			dprintf(2, "Giving up.\n");
			break ;
		}
		else
			dprintf(2, "Retrying in %ds...\n", (g_ms << retries++) / 1000);
	return (s > 0 && (pfd.revents & POLLOUT));
}

int			receive_n_retry(int socket, struct s_packet **queue)
{
	struct sockaddr	*local = (struct sockaddr *)&g_shell->daemon.addr.unix;
	int				s;
	int				send;
	int				retries;

	retries = 0;
	g_to.tv_sec = 1;
	setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &g_to, sizeof(g_to));
	setsockopt(socket, SOL_SOCKET, SO_SNDBUF, &(int){0}, sizeof(int));
	while ((s = connect(socket, local, sizeof(struct sockaddr_un))) &&
			(send = net_send(socket, g_handshake)) != -1 &&
			(s = net_get(socket, queue)) == 0)
		if (retries + 1 == MAX_RETRIES)
		{
			dprintf(2, "Giving up.\n");
			break ;
		}
		else
		{
			dprintf(2, "Retrying in %ld seconds... %s\n", (g_to.tv_sec <<= 1), strerror(errno));
			setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &g_to, sizeof(g_to));
			retries++;
		}
	return (send == 0 && s && errno == 0);
}

int			_check_connection(int socket)
{
	struct s_packet	*queue;
	json_object		*root;
	int				s;

	if (g_handshake == NULL)
	{
		root = json_object_new_object();
		json_object_object_add(root, "handshake",
			json_object_new_string("Hello, nice to meet you."));
		g_handshake = packet_create_json(root, REQUEST_HANDSHAKE, NULL);
	}
	s = 0;
	queue = NULL;
	errno = 0;
	if ((s = net_send(socket, g_handshake)) == -1)
		return ((dprintf(2, "Failed to connect to the daemon: %s\n",
			 strerror(errno == 0 ? EHOSTDOWN : errno)) & 0) | 1);
	else
		s = receive_n_retry(socket, &queue);
	packet_resolve_all(queue);
	return (s && errno == 0);
}

// TODO: use sysctl() to retrieve information about running taskmasterd processes
static int	connect_socket_unix(void)
{
	struct sockaddr_un		*local;
	struct s_remote			*daemon;
	int						connection;

	daemon = &g_shell->daemon;
	local = memset(&daemon->addr.unix, 0, sizeof(struct sockaddr_un));
	local->sun_family = AF_LOCAL;
	daemon->domain = AF_LOCAL;
	strncpy(local->sun_path, SOCKET_FILE, sizeof(local->sun_path) - 1);
	if ((daemon->socket = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1)
		dprintf(2, "%s\nsocket %s\n", strerror(errno), SOCKET_FILE);
	if ((connection = connect(daemon->socket, (struct sockaddr *)local,
		sizeof(local->sun_path))) == -1)
		dprintf(2, "%s: socket %s\n", strerror(errno), SOCKET_FILE);
	else if ((daemon->is_alive = _check_connection(daemon->socket)))
	{
		setsockopt(daemon->socket, SOL_SOCKET, SO_KEEPALIVE, &(int){1}, sizeof(int));
		printf("Connection successful. Socket %s\n", SOCKET_FILE);
	}
	else
	{
		bzero(daemon, sizeof(struct s_remote));
		dprintf(2, "Connection failed. Check if daemon is running and retry\n");
	}
	return (connection);
}

int			tm_connect(const char **args)
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
	}
	else if (!args || !args[0])
	{
		ft_dprintf(2, "usage: connect [unix <pid> | local <pid> | inet <ip>]\n");
		return (1);
	}
	else if (strcmp(args[0], "unix") == 0)
		return (connect_socket_unix());
	return (0);
}
