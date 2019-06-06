/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sockets_manipulations.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/21 13:47:28 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/05 21:35:41 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <assert.h>
#include "taskmaster_daemon.h"

void				destroy_socket(struct s_socket **sock)
{
	close((*sock)->fd);
	free(*sock);
	*sock = NULL;
}

struct s_socket		*socket_create_inet(const char *address)
{
	struct s_socket		*sock;

	sock = calloc(1, sizeof(struct s_socket));
	sock->socket_domain = AF_INET;
	sock->addr.inet.sin_family = AF_INET;
	if ((sock->fd = socket(AF_INET, SOCK_STREAM, 0)))
		log_fwrite(TLOG_ERROR, "Failed to create a local socket: %s", strerror(errno));
	else if (bind(sock->fd, (struct sockaddr *)&sock->addr.inet, sizeof(struct sockaddr_in)) == -1)
		log_fwrite(TLOG_ERROR, "Failed to bind a socket to %s", address);
	destroy_socket(&sock);
	return (sock);
}

struct s_socket		*socket_create_local(const char *filename)
{
	struct s_socket		*sock;

	sock = calloc(1, sizeof(struct s_socket));
	strncpy(sock->addr.local.sun_path, filename, sizeof(sock->addr.local.sun_path) - 1);
	sock->socket_domain = AF_LOCAL;
	sock->addr.local.sun_family = AF_LOCAL;
	if ((sock->fd = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1)
	{
		log_fwrite(TLOG_ERROR, "Failed to create a local socket: %s: %s", filename, strerror(errno));
		destroy_socket(&sock);
	}
	else if (bind(sock->fd, (struct sockaddr *)&sock->addr.local, sizeof(struct sockaddr_un)) == -1)
	{
		log_fwrite(TLOG_ERROR, "Failed to bind a socket to %s: %s", filename, strerror(errno));
		destroy_socket(&sock);
	}
	else
	{
		log_fwrite(TLOG_DEBUG, "Successfully created socket at %s (fd: %d)", filename, sock->fd);
		if (setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
			log_fwrite(TLOG_ERROR, "Failed to set socket options: %s", strerror(errno));
	}
	return (sock);
}
