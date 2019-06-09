/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tm_connect.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/20 19:36:29 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/09 18:53:44 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <setjmp.h>
#include "taskmaster_cli.h"

#define SOCKET_FILE "/var/tmp/taskmasterd.socket"
#define MAX_RETRIES 5

static struct s_packet	*g_handshake;
static jmp_buf			g_connection_jmp;
static int				g_retries;

static inline int	receive_n_retry(int sock, struct s_packet **queue)
{
	const struct timeval		t = {4, 0};
	const struct sockaddr_un	*local = &g_shell->daemon.addr.unix;

	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t));
	setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &(int){0}, sizeof(int));
	if ((connect(sock, (struct sockaddr *)local, sizeof(*local)) == 0))
	{
		if ((net_send(sock, g_handshake)) != -1 && g_retries + 1 == MAX_RETRIES)
			return (dprintf(2, "Giving up.\n") & 0);
		else if ((net_get(sock, queue)) < 0 && g_retries++ < MAX_RETRIES)
		{
			dprintf(2, "Retrying in %ld seconds...\n", (t.tv_sec));
			close(sock);
			longjmp(g_connection_jmp, 1);
		}
		else
			return (true);
	}
	return (false);
}

static int			check_connection(int socket)
{
	struct s_packet	*queue;
	json_object		*root;
	int				s;

	if (g_handshake == NULL)
	{
		root = json_object_new_object();
		json_object_object_add(root, "handshake", json_object_new_string(HANDSHAKE_REQUEST));
		g_handshake = packet_create_json(root, REQUEST_HANDSHAKE, NULL);
	}
	queue = NULL;
	errno = 0;
	s = receive_n_retry(socket, &queue);
	packet_resolve_all(queue);
	return (s);
}

static int			connect_socket_unix(void)
{
	struct sockaddr_un		*local;
	struct s_remote			*daemon;

	g_retries = 0;
	daemon = &g_shell->daemon;
	local = memset(&daemon->addr.unix, 0, sizeof(struct sockaddr_un));
	local->sun_family = AF_LOCAL;
	daemon->domain = AF_LOCAL;
	strncpy(local->sun_path, SOCKET_FILE, sizeof(local->sun_path) - 1);
	setjmp(g_connection_jmp);
	if ((daemon->socket = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1)
		dprintf(2, "%s\nsocket %s\n", strerror(errno), SOCKET_FILE);
	if ((daemon->is_alive = check_connection(daemon->socket)))
	{
		setsockopt(daemon->socket, SOL_SOCKET, SO_KEEPALIVE, &(int){1}, sizeof(int));
		setsockopt(daemon->socket, SOL_SOCKET, SO_SNDBUF, &(int){2048}, sizeof(int));
		printf("Connection successful. Socket %s\n", SOCKET_FILE);
	}
	else
	{
		bzero(daemon, sizeof(struct s_remote));
		dprintf(2, "Connection failed. Check if daemon is running and retry\n");
	}
	return (!daemon->is_alive);
}

int					tm_connect(const char **args)
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
