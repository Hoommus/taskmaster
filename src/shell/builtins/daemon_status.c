/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   daemon_status.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/24 12:34:01 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/29 17:29:28 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"

char		*pack_to_json_str(enum e_request type, const char **args)
{
	json_object		*object;
	json_object		*tmp;
	const char		*final;
	int				i;

	object = json_object_new_object();
	json_object_object_add(object, "request", json_object_new_int(type));
	tmp = json_object_new_array();
	i = 0;
	while (args && args[i])
		json_object_array_add(tmp, json_object_new_string(args[i++]));
	json_object_object_add(object, "argv", tmp);
	final = json_object_to_json_string(object);
	json_object_put(object);
	return ((char *)final);
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
	if (json_object_object_get_ex(packet->json_content, "jobs", &value))
	{
		i = -1;
		len = json_object_array_length(value);
		while (++i < len)
		{
			job = json_object_array_get_idx(value, i);
			printf("name: %s\n", json_object_get_string(json_object_object_get(job, "name")));
			printf("%-20s %d\n", "instances:", json_object_get_int(json_object_object_get(job, "instances")));
		}
	}
	//json_object_put(object); TODO: Consider freeing of objects
	return (status);
}

int			tm_status(const char **args)
{
	char	*request;
	char	*response;
	int		status;

	status = -1;
	response = NULL;
	if (args && args[0] && strcmp(args[0], "--help") == 0)
		printf("Usage goes here\n");
	else
	{
		request = pack_to_json_str(REQUEST_STATUS, args);
		if (net_send(g_shell->daemon->socket_fd, request) != 0)
			dprintf(2, SH ": failed to send request: %s\n", strerror(errno));
		else if (net_get(g_shell->daemon->socket_fd) == 0)
			status = packet_resolve_first(REQUEST_STATUS);
		free((void *)response);
		free((void *)request);
		return (status);
	}
	return (0);
}
