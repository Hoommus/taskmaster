/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serve_invalid.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/06 16:52:36 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/08 13:36:31 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"

int							serve_invalid(const struct s_packet *packet)
{
	struct s_packet	*response;
	json_object		*root;

	log_write(TLOG_DEBUG, "Received invalid request");
	root = json_object_new_object();
	json_object_object_add(root, "error", json_object_new_string("bad request"));
	response = packet_create_json(root, packet->request, NULL);
	if (net_send(packet->respond_to, response) == -1)
		log_fwrite(TLOG_WARN, "Failed to answer a rogue client: %s", strerror(errno));
	packet_free(&response);
	return (packet == NULL);
}
