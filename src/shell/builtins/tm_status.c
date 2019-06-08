/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tm_status.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/24 12:34:01 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/08 12:15:14 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"

static struct s_packet		*pack_to_packet(enum e_request type, const char **args)
{
	json_object		*object;
	json_object		*tmp;
	int				i;

	object = json_object_new_object();
	json_object_object_add(object, "request", json_object_new_int(type));
	tmp = json_object_new_array();
	i = 0;
	while (args && args[i])
		json_object_array_add(tmp, json_object_new_string(args[i++]));
	json_object_object_add(object, "argv", tmp);
	return (packet_create_json(object, type, NULL));
}

int			parse_response_status(const struct s_packet *packet)
{
	json_object			*value;
	json_object			*job;
	int					status;
	int					i;
	int					len;

	status = -1;
	if (json_object_object_get_ex(packet->json_content, "status", &value))
		status = json_object_get_int(value);
	json_object_put(value);
	if (json_object_object_get_ex(packet->json_content, "jobs", &value)
		&& (len = json_object_array_length(value)) > 0)
	{
		i = -1;
		while (++i < len)
		{
			job = json_object_array_get_idx(value, i);
			printf("name: %s\n", json_object_get_string(json_object_object_get(job, "name")));
			printf("%-20s %d\n", "instances:", json_object_get_int(json_object_object_get(job, "instances")));
		}
	}
	else
		printf("No jobs.\n");
	return (status);
}

int			tm_status_help(void)
{
	return (printf("status:\nrequests data about jobs from the daemon "
				"and prints it nicely\n"));
}

int			tm_status(const char **args)
{
	struct s_packet	*request;
	struct s_packet	*queue;
	char			*response;
	int				status;

	response = NULL;
	if (args && args[0] && strcmp(args[0], "--help") == 0)
		printf("Usage goes here\n");
	else if (check_connection(g_shell->daemon.socket))
	{
		request = pack_to_packet(REQUEST_STATUS, args);
		errno = 0;
		if ((status = net_send(g_shell->daemon.socket, request)) < 0)
			dprintf(2, SH ": failed to send request: %s\n",
				errno == EPIPE ? "server dropped connection" : strerror(errno));
		else if (status == 0 && errno != 0)
			dprintf(2, SH ": request timed out");
		else
		{
			queue = NULL;
			if (net_get(g_shell->daemon.socket, &queue) >= 0)
				status = packet_resolve_all(queue);
			else
				dprintf(2, SH ": response timed out");
		}
		free((void *)response);
		packet_free(&request);
		return (status);
	}
	return (0);
}
