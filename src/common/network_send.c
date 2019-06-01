/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network_send.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/28 11:45:45 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/01 18:36:08 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_printf.h>
#include "taskmaster_common.h"

int			net_send(int socket, struct s_packet *packet)
{
	const char		*copy;
	const char		*swap;
//	ssize_t			last_write;
	size_t			packet_left;

	copy = json_object_to_json_string(packet->json_content);
	ft_dprintf(2, "%s\n", copy);
	swap = copy;
	packet_left = strlen(copy);
	write(socket, copy, packet_left);
//	while ((last_write = ))
//	{
//		if (last_write == -1 && (errno == EINTR || errno == EAGAIN))
//			continue ;
//		else if (last_write == -1 && (errno == EPIPE || errno == ENETDOWN ||
//								errno == ENETUNREACH || errno == ENETRESET))
//			return (-1);
//		copy += last_write;
//		packet_left -= last_write;
//	}
	free((void *)swap);
	(write(socket, "\23", 1));
	return (0);
}

