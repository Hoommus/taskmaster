/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_interactions.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/29 18:37:19 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/30 16:00:41 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"

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

void		accept_receive_respond_loop(void)
{
	struct sockaddr_storage		client;
	u_int32_t					client_size;
	int							client_fd;

	bzero(&client, sizeof(struct sockaddr_storage));
	client_size = sizeof(client);
	client_fd = -1;
	while (ponies_teleported())
	{
		if (client_fd <= 0)
		{
			client_fd = accept(g_master->sockets[0]->fd, (struct sockaddr *)&client, &client_size);
			if (errno != EINTR && errno != EAGAIN)
				dprintf(g_master->logfile, "Connection acceptance failed\n");
		}
		if (client_fd != -1 && errno == EAGAIN)
		{
			net_get(client_fd);
			packet_resolve_all();
		}
		else
			dprintf(g_master->logfile, "New client connected on fd %d\n", client_fd);
	}
}

