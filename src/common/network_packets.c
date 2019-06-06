/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network_packets.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/31 13:19:08 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/06 20:15:55 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_common.h"

struct s_packet	*packet_create(int socket, const char *contents, struct timeval timestamp)
{
	json_object			*obj;
	struct s_packet		*packet;

	packet = calloc(1, sizeof(struct s_packet));
	if (packet == NULL)
		return (NULL);
	packet->respond_to = socket;
	packet->timestamp = timestamp;
	if ((obj = json_tokener_parse(contents)))
	{
		packet->json_content = obj;
		packet->is_content_parsed = true;
		packet->request = json_object_get_int(json_object_object_get(obj, "request"));
		json_object_object_add(obj, "packet_time_sec", json_object_new_int64(timestamp.tv_sec));
		json_object_object_add(obj, "packet_time_usec", json_object_new_int64(timestamp.tv_usec));
	}
	else
	{
		packet->content = NULL;
		packet->json_content = NULL;
		packet->request = 2147483647;
	}
	return (packet);
}

struct s_packet	*packet_create_json(json_object *content,
									enum e_request request,
									const struct timeval timestamp)
{
	struct s_packet		*packet;

	packet = calloc(1, sizeof(struct s_packet));
	if (packet == NULL)
		return (NULL);
	packet->timestamp = timestamp;
	packet->json_content = content;
	packet->is_content_parsed = true;
	json_object_object_add(content, "request", json_object_new_int(request));
	json_object_object_add(content, "packet_time_sec", json_object_new_int64(timestamp.tv_sec));
	json_object_object_add(content, "packet_time_usec", json_object_new_int64(timestamp.tv_usec));
	packet->request = request;
	return (packet);
}

/*
** Doesn't take into account .next and .json_content fields
** And why the fuck I even need this?
*/

int				packets_equal(struct s_packet *a, struct s_packet *b)
{
	if (a == b)
		return (true);
	if (a == NULL || b == NULL)
		return (false);
	return (a->timestamp.tv_sec == b->timestamp.tv_sec &&
			a->timestamp.tv_usec == b->timestamp.tv_usec &&
			a->is_content_parsed == b->is_content_parsed &&
			a->request == b->request &&
			(a->content == b->content || strcmp(a->content, b->content) == 0));
}

int				packet_free(struct s_packet **packet)
{
	if (packet == NULL || *packet == NULL)
		return (1);
	free((*packet)->content);
//	if ((*packet)->is_content_parsed && (*packet)->json_content)
//		json_object_put((*packet)->json_content);
	free(*packet);
	*packet = NULL;
	return (0);
}
