/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network_send.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/28 11:45:45 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/07 11:33:48 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_common.h"

int			net_send(int socket, struct s_packet *packet)
{
	const char		*copy;
	ssize_t			last_write;
	ssize_t			packet_left;
	struct timeval	timeout;

	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	copy = json_object_to_json_string(packet->json_content);
	packet_left = strlen(copy);
	while ((last_write = write(socket, copy, packet_left)) != packet_left)
	{
		if (last_write == -1 && (errno == EINTR || errno == EAGAIN))
			continue ;
		else if (last_write == -1 && (errno == EPIPE || errno == ENETDOWN ||
								errno == ENETUNREACH || errno == ENETRESET))
			return (-1);
		copy += last_write;
		packet_left -= last_write;
	}
	(write(socket, "\23", 1));
	return (0);
}

