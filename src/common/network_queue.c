/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network_queue.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/27 18:40:07 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/31 16:21:25 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"

static struct s_packet	*g_queue_head;
static struct s_packet	*g_queue_tail;

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

int				packet_dequeue(struct s_packet *packet)
{
	struct s_packet	*list;
	struct s_packet	*prev;

	prev = NULL;
	list = g_queue_head;
	while (list)
	{
		if (list->request == packet->request &&
			list->timestamp.tv_sec == packet->timestamp.tv_sec)
		{
			if (prev == NULL)
				g_queue_head = list->next;
			else
				prev->next = packet->next;
			packet_free(&packet);
			return (0);
		}
		prev = list;
		list = list->next;
	}
	return (1);
}

int				packet_resolve_all(void)
{
	struct s_packet	*list;
	struct s_packet	*backup;
	int				i;

	i = 0;
	list = g_queue_head;
	while (list)
	{
		while (g_resolvers[i].resolver)
		{
			if (g_resolvers[i].command == list->request)
			{
				g_resolvers[i].resolver(list);
				backup = list;
			}
			i++;
		}
		list = list->next;
		packet_free(&backup);
	}
	g_queue_head = NULL;
	g_queue_tail = NULL;
	return (0);

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
