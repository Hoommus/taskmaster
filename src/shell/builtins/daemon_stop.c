/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   daemon_stop.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/06 18:17:55 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/06 21:36:42 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"

static struct s_packet	*pack_to_packet(enum e_request type, const char **args)
{
	json_object		*object;
	struct timeval	time;

	object = json_object_new_object();
	json_object_object_add(object, "job_name", json_object_new_string(args[0]));
	gettimeofday(&time, NULL);
	return (packet_create_json(object, type, time));
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
		printf("usage: stop [all|name]\n");
	else if (g_shell->daemon == NULL)
		dprintf(2, "You need to connect to some daemon first\n");
	else
	{
		request = pack_to_packet(REQUEST_STOP, args);
		dprintf(2, SH ": sending request...\n");
		if (net_send(g_shell->daemon->socket_fd, request) != 0)
			dprintf(2, SH ": failed to send request: %s\n", strerror(errno));
		else
		{
			dprintf(2, SH ": seems like sent all, now waiting for response\n");
			if (net_get(g_shell->daemon->socket_fd, &queue) >= 0)
				status = packet_resolve_all(queue);
		}
		free((void *)response);
		return (status);
	}
	return (0);
}