/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network_send.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/28 11:45:45 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/28 16:10:58 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"

int			net_send(int socket, const char *request)
{
	const char		*copy;
	ssize_t			last_write;
	size_t			packet_left;

	copy = request;
	packet_left = strlen(request);
	while ((last_write = write(socket, copy, packet_left)))
	{
		if (last_write == -1 && (errno == EINTR || errno == EAGAIN))
			continue ;
		else if (last_write == -1 && (errno == EPIPE || errno == ENETDOWN ||
								errno == ENETUNREACH || errno == ENETRESET))
			return (-1);
		copy += last_write;
		packet_left = strlen(copy);
	}
	write(socket, "\4", 1);
	return (0);
}

