/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heartbeat.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/06 12:08:11 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/06 12:08:14 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_common.h"

int			send_heartbeat(struct s_packet *packet)
{
	struct timeval	val;

	gettimeofday(&val, NULL);
	packet_create_json(json_object_new_object(), REQUEST_HEARTBEAT, val);
	return (0);
}
