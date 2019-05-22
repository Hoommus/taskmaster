/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sockets_manipulations.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/21 13:47:28 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/22 15:44:05 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <assert.h>
#include "taskmaster_daemon.h"

void				send_heartbeat(t_socket *socket)
{
	write(socket->fd, "\0\0\0\0", 4);
}

/*
** Creates socket and binds it to desired address
*/

struct s_socket		*create_socket(int domain, const char *filename, __unused const char *address)
{
	struct s_socket		*sock;

	assert(domain == AF_INET || domain == AF_LOCAL);
	sock = calloc(1, sizeof(struct s_socket));
	sock->socket_domain = domain;
	if (domain == AF_LOCAL)
	{
		sock->addr.unix.sun_family = domain;
		strncpy(sock->addr.unix.sun_path, filename, sizeof(sock->addr.unix.sun_path) - 1);
	}
	else if (domain == AF_INET)
	{
		sock->addr.inet.sin_family = domain;
		// TODO: look into what inet socket address needs and make this happen
		// strncpy(sock->addr.inet.sin_addr.s_addr, filename, sizeof(sock->addr.inet.sin_addr) - 1);
	}
	sock->fd = socket(domain, SOCK_STREAM, 0);
	if (domain == AF_LOCAL)
		if (bind(sock->fd, (struct sockaddr *)&sock->addr.unix,
			sizeof(struct sockaddr_un)) == -1)
			dprintf(g_master->logfile, "Binding socket to %s failed:\n%s\n",
				sock->addr.unix.sun_path, strerror(errno));
	if (sock->fd == -1)
		dprintf(g_master->logfile, "Socket creation failed for %s\n", filename);
	return (sock);
}

void				destroy_socket(struct s_socket **sock)
{
	close((*sock)->fd);
	free(*sock);
	*sock = NULL;
}
