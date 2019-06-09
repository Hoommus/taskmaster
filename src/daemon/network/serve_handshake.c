/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serve_handshake.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/08 13:31:46 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/08 19:27:43 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"

int							serve_handshake(const struct s_packet *packet)
{
	struct s_packet	*response;
	json_object		*root;

	log_write(TLOG_DEBUG, "Received handshake request");
	root = json_object_new_object();
	json_object_object_add(root, "handshake", json_object_new_string(HANDSHAKE_RESPONSE));
	response = packet_create_json(root, packet->request, NULL);
	if (net_send(packet->respond_to, response) == -1)
		log_fwrite(TLOG_WARN, "Failed to answer to a handshake request: %s", strerror(errno));
	else
		log_write(TLOG_DEBUG, "Successfully responded to a handshake");
	packet_free(&response);
	return (0);
}
