/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network_queue.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/27 18:40:07 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/30 17:06:52 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"

static struct s_packet	*g_queue_head;
static struct s_packet	*g_queue_tail;

struct s_packet	*packet_create(const char *contents, struct timeval timestamp)
{
	json_object			*obj;
	struct s_packet		*packet;

	packet = calloc(1, sizeof(struct s_packet));
	if (packet == NULL)
		fatal("out of memory");
	packet->timestamp = timestamp;
	if ((obj = json_tokener_parse(contents)))
	{
		packet->json_content = obj;
		packet->is_content_parsed = true;
		packet->request = json_object_get_int(json_object_object_get(obj, "request"));
	}
	else
	{
		packet->content = strdup(contents);
		packet->request = 2147483647;
	}
	return (packet);
}

// TODO: Limit packet list size

int				packet_enqueue(struct s_packet *packet)
{
	if (g_queue_head == NULL)
		g_queue_head = packet;
	else if (g_queue_head != NULL && g_queue_tail == NULL)
	{
		g_queue_head->next = packet;
		g_queue_tail = packet;
	}
	else if (g_queue_head != NULL && g_queue_tail != NULL)
		g_queue_tail->next = packet;
	return (0);
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

int				packet_dequeue(struct s_packet *packet)
{
	struct s_packet	*list;
	struct s_packet	*prev;

	prev = NULL;
	list = g_queue_head;
	while (list)
	{
		if (packets_equal(list, packet))
		{
			if (prev == NULL)
				g_queue_head = list->next;
			else
				prev->next = packet->next;
			packet_destroy(&packet);
			return (0);
		}
		prev = list;
		list = list->next;
	}
	return (packet == NULL);
}

int				packet_resolve_first(enum e_request type)
{
	int						i;
	const struct s_packet	*list;

	i = 0;
	list = g_queue_head;
	while (g_resolvers[i].resolver)
	{
		if (g_resolvers[i].command == list->request && list->request == type)
			return (g_resolvers[i].resolver(list));
		i++;
	}
	return (1);
}

int				packet_destroy(struct s_packet **packet)
{
	free((*packet)->content);
	if ((*packet)->is_content_parsed && (*packet)->json_content)
		json_object_put((*packet)->json_content);
	free(*packet);
	*packet = NULL;
	return (0);
}
