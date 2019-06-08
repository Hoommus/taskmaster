/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tm_stop.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/06 18:17:55 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/07 22:29:48 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"

static struct s_packet	*pack_to_packet(enum e_request type, const char **args)
{
	json_object		*object;

	object = json_object_new_object();
	json_object_object_add(object, "job_name", json_object_new_string(args[0]));
	return (packet_create_json(object, type, NULL));
}

// $ stop foo:foo_1

int						tm_stop(const char **args)
{
	struct s_packet	*request;
	struct s_packet	*queue;
	char			*response;
	int				status;

	status = -1;
	response = NULL;
	if (args && ((args[0] && strcmp(args[0], "--help") == 0) || args[0] == NULL))
		return ((dprintf(2, "usage: stop [all|name]\n") & 0) | 1);
	request = pack_to_packet(REQUEST_STOP, args);
	if (net_send(g_shell->daemon.socket, request) != 0)
		dprintf(2, SH ": failed to send request: %s\n", strerror(errno));
	else
	{
		queue = NULL;
		if (net_get(g_shell->daemon.socket, &queue) >= 0)
			status = packet_resolve_all(queue);
	}
	free((void *)response);
	return (status);
}