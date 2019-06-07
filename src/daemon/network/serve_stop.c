/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serve_stop.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/06 17:45:55 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/07 17:18:21 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"

int							respond_stop(const struct s_packet *packet)
{
	struct timeval	time;
	struct s_packet	*response;
	json_object		*root;
	const char		*job_name;

	job_name = json_object_get_string(json_object_object_get(packet->json_content, "job_name"));
	log_fwrite(TLOG_INFO, "Client requested %s job stop", job_name);

	d_stop(0, 0);

	root = json_object_new_object();
	json_object_object_add(root, "success", json_object_new_int(1));
	gettimeofday(&time, NULL);
	response = packet_create_json(root, packet->request, time);
	if (net_send(packet->respond_to, response) == -1)
		log_fwrite(TLOG_WARN, "Status reporting failed: %s", strerror(errno));
	else
		log_write(TLOG_INFO, "Successfully reported status to client");
	packet_free(&response);
	return (packet == NULL);
}
