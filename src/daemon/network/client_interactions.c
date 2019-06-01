/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_interactions.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/29 18:37:19 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/01 20:43:40 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"

static int			socket_fdddd;

struct s_resolver	g_resolvers[] = {
	{REQUEST_STATUS, &respond_status},
	{0, NULL}
};

int					respond_status(const struct s_packet *packet)
{
	struct s_packet	*response;
	json_object		*root;
	json_object		*array;
	struct timeval	time;

	root = json_object_new_object();
	array = json_object_new_array();
	json_object_object_add(root, "jobs", (array));
	gettimeofday(&time, NULL);
	response = packet_create_json(root, packet->request, time);
	dprintf(g_master->logfile, "Reporting status to client\n");
	net_send(socket_fdddd, response);
	return (packet == NULL);
}

static ssize_t		ponies_teleported(void)
{
	ssize_t			ponies;
	int				fd;

	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
		return (1);
	else
	{
		read(fd, &ponies, sizeof(ssize_t));
		if (ponies == 0)
			ponies += 1348;
		close(fd);
		return (ABS(ponies));
	}
}

void				accept_receive_respond_loop(void)
{
	struct sockaddr_storage		client;
	u_int32_t					client_size;
	int							client_fd;

	bzero(&client, sizeof(struct sockaddr_storage));
	client_size = sizeof(client);
	client_fd = -1;
	dup2(g_master->logfile, 2);
	while (ponies_teleported())
	{
		if (client_fd <= 0)
		{
			// socket fd here should not have O_NONBLOCK
			client_fd = accept(g_master->sockets[0]->fd,
				(struct sockaddr *)&client, &client_size);
			if (client_fd < 0 && errno != EINTR && errno != EAGAIN)
				dprintf(g_master->logfile, "Connection acceptance failed\n");
			else
				dprintf(g_master->logfile, "Connected new client\n");
			errno = 0;
		}
		if (client_fd != -1)
		{
			dprintf(g_master->logfile, "Waiting for requests\n");
			if (net_get(client_fd) >= 0)
			{
				socket_fdddd = client_fd;
				dprintf(g_master->logfile, "Got some requests\n");
				packet_resolve_all();
			}
			else
			{
				dprintf(g_master->logfile, "net_get returned error: %s\n", strerror(errno));
			}
		}
	}
}
