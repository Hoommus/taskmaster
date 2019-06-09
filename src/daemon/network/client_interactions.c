/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_interactions.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/29 18:37:19 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/08 15:06:22 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"

struct s_resolver			g_resolvers[] = {
	{REQUEST_STATUS, &serve_status},
	{REQUEST_STOP, &serve_stop},
	{REQUEST_HANDSHAKE, &serve_handshake},
	{2147483647, &serve_invalid},
	{0, NULL}
};

void __attribute__((noreturn))	*receive_respond(void *arg)
{
	const struct s_thread_args	*args = (struct s_thread_args *)arg;
	struct s_packet				*queue;

	// TODO: get some info about the client
	log_write(TLOG_INFO, "Started listening packets");
	while (net_get(args->socket_fd, &queue))
	{
		log_write(TLOG_INFO, "Received some packets, resolving requests");
		packet_resolve_all(queue);
		queue = NULL;
	}
	log_write(TLOG_INFO, "Client dropped connection");
	close(args->socket_fd);
	tpool_finalize_thread(args->thread_id);
	pthread_exit(NULL);
}

void __attribute__((noreturn))	*accept_pthread_loop(void *socket)
{
	const int					sock = *((int *)socket);
	struct sockaddr_storage		client;
	u_int32_t					client_size;
	int							client_fd;
	int							socket_flags;

	log_fwrite(TLOG_DEBUG, "Started thread for %d socket", sock);
	bzero(&client, sizeof(struct sockaddr_storage));
	client_size = sizeof(client);
	socket_flags = fcntl(sock, F_GETFL);
	fcntl(sock, F_SETFL, socket_flags & ~(O_NONBLOCK));
	while (ponies_teleported())
	{
		client_fd = accept(sock, (struct sockaddr *)&client, &client_size);
		if (client_fd < 0 && errno != EINTR && errno != EAGAIN)
		{
			log_fwrite(TLOG_WARN, "Connection acceptance failed: %s: %d", strerror(errno), sock);
			usleep(5000000);
		}
		else if (client_fd > 0)
		{
			log_fwrite(TLOG_INFO, "Connected new %s client",
				client.ss_family == AF_LOCAL ? "local" : "remote");
			if (tpool_create_thread(NULL, receive_respond, tpool_arg(client_fd)) < 0)
			{
				log_write(TLOG_INFO, "Rejected new client: out of client threads");
				close(sock);
			}
		}
	}
	pthread_exit(NULL);
}
