/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network_queue.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/27 18:40:07 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/06 22:10:21 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_common.h"

int				packet_enqueue(struct s_packet **queue_head,
								struct s_packet *packet)
{
	struct s_packet		*list_copy;

	if (queue_head == NULL)
		return (-1);
	if (*queue_head == NULL)
		*queue_head = packet;
	else
	{
		list_copy = *queue_head;
		while (list_copy->next)
			list_copy = list_copy->next;
		list_copy->next = packet;
	}
	return (0);
}

int				packet_resolve_all(struct s_packet *queue)
{
	struct s_packet	*backup;
	int				i;

	while (queue)
	{
		i = 0;
		while (g_resolvers[i].resolver)
		{
			if (g_resolvers[i].command == queue->request)
			{
				g_resolvers[i].resolver(queue);

			}
			i++;
		}
		backup = queue;
		queue = queue->next;
		//packet_free(&backup);
	}
	return (0);
}
