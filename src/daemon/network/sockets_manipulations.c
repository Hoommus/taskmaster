/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sockets_manipulations.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/21 13:47:28 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/04 16:51:57 by vtarasiu         ###   ########.fr       */
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
		dprintf(g_master->logfile, "Failed to create a local socket: %s\n", strerror(errno));
	else if (bind(sock->fd, (struct sockaddr *)&sock->addr.inet, sizeof(struct sockaddr_in)) == -1)
		dprintf(g_master->logfile, "Failed to bind a socket to %s\n", address);
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
	if ((sock->fd = socket(AF_LOCAL, SOCK_STREAM, 0)))
		dprintf(g_master->logfile, "Failed to create a local socket: %s\n", strerror(errno));
	else if (bind(sock->fd, (struct sockaddr *)&sock->addr.local, sizeof(struct sockaddr_un)) == -1)
		dprintf(g_master->logfile, "Failed to bind a socket to %s\n", filename);
	destroy_socket(&sock);
	return (sock);
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
		sock->addr.local.sun_family = domain;
		strncpy(sock->addr.local.sun_path, filename, sizeof(sock->addr.local.sun_path) - 1);
	}
	else if (domain == AF_INET)
	{
		sock->addr.inet.sin_family = domain;
		// TODO: look into what inet socket address needs and make this happen
		// strncpy(sock->addr.inet.sin_addr.s_addr, filename, sizeof(sock->addr.inet.sin_addr) - 1);
	}
	sock->fd = socket(domain, SOCK_STREAM, 0);
	if (domain == AF_LOCAL)
		if (bind(sock->fd, (struct sockaddr *)&sock->addr.local,
			sizeof(struct sockaddr_un)) == -1)
			dprintf(g_master->logfile, "Binding socket to %s failed:\n%s\n",
				sock->addr.local.sun_path, strerror(errno));
	if (sock->fd == -1)
	{
		dprintf(g_master->logfile, "Socket creation failed for %s: %s\n", filename, strerror(errno));
		destroy_socket(&sock);
		return (NULL);
	}
	return (sock);
}

