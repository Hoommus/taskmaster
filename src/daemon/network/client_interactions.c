/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_interactions.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/29 18:37:19 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/03 18:24:18 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"

struct s_resolver			g_resolvers[] = {
	{REQUEST_STATUS, &respond_status},
	{0, NULL}
};

int							respond_status(const struct s_packet *packet)
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
	net_send(packet->respond_to, response);
	// No need to free json object
	return (packet == NULL);
}

void __attribute__((noreturn))	*receive_respond(void *arg)
{
	const struct s_thread_args	*args = (struct s_thread_args *)arg;

	// TODO: get some info about the client
	while (net_get(args->socket_fd))
		packet_resolve_all();
	dprintf(g_master->logfile, "Client dropped connection\n");
	close(args->socket_fd);
	tpool_finalize_thread(args->thread_id);
	pthread_exit(NULL);
}

// TODO: add socket timeouts
void __attribute__((noreturn))	*accept_pthread_loop(void *socket)
{
	const int					sock = *((int *)socket);
	struct sockaddr_storage		client;
	u_int32_t					client_size;
	int							client_fd;
	int							socket_flags;

	bzero(&client, sizeof(struct sockaddr_storage));
	client_size = sizeof(client);
	dup2(g_master->logfile, 2);
	socket_flags = fcntl(sock, F_GETFL);
	fcntl(sock, F_SETFL, socket_flags & ~(O_NONBLOCK));
	while (ponies_teleported())
	{
		client_fd = accept(sock, (struct sockaddr *)&client, &client_size);
		if (client_fd < 0 && errno != EINTR && errno != EAGAIN)
			dprintf(g_master->logfile, "Connection acceptance failed: %s: %d\n", strerror(errno), sock);
		else if (client_fd > 0)
		{
			dprintf(g_master->logfile, "Connected new client %%add some information here%%\n");
			if (tpool_create_thread(NULL, receive_respond, tpool_arg(client_fd)) < 0)
			{
				dprintf(g_master->logfile, "Rejected new client: out of client threads\n");
				close(sock); // reject connection
			}
		}
	}
	pthread_exit(NULL);
}
