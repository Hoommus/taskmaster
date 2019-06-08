/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serve_status.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/06 16:52:06 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/06 20:15:55 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"

int							serve_status(const struct s_packet *packet)
{
	struct s_packet	*response;
	json_object		*root;
	json_object		*array;

	log_fwrite(TLOG_DEBUG, "Reporting status to a client, packet: %p", packet);
	if (packet == NULL)
	{
		log_write(TLOG_FATAL, "packet is null");
		exit(1);
	}
	root = json_object_new_object();
	array = json_object_new_array();
	json_object_object_add(root, "jobs", (array));
	response = packet_create_json(root, packet->request, NULL);
	if (net_send(packet->respond_to, response) == -1)
		log_fwrite(TLOG_WARN, "Status reporting failed: %s", strerror(errno));
	packet_free(&response);
	log_write(TLOG_INFO, "Successfully reported status to client");
	return (packet == NULL);
}
