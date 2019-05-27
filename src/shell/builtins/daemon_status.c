/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   daemon_status.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/24 12:34:01 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/26 17:07:08 by vtarasiu         ###   ########.fr       */
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

int			try_sending_all(const char *request)
{
	const int		socket = g_shell->daemon->socket_fd;
	const char		*copy;
	ssize_t			last_write;
	size_t			packet_left;

	copy = request;
	packet_left = strlen(request);
	while ((last_write = write(socket, copy, packet_left)))
	{
		if (last_write == -1)
			continue ;
		copy += last_write;
		packet_left = strlen(copy);
	}
	return (0);
}

int			try_receive_response(char **result)
{
	char		*response;
	char		*tmp;
	char		buffer[1024];
	int			retries;
	ssize_t		status;

	status = 0;
	retries = 0;
	response = ft_strnew(0);
	while (retries < 5 && memset(buffer, 0, sizeof(buffer)))
		if ((status = read(g_shell->daemon->socket_fd, buffer, 1023)) == -1)
			sleep(1 << retries++);
		else if (status > 0)
		{
			tmp = ft_strings_join(2, "", response, buffer);
			free((void *)response);
			response = tmp;
			retries = 0;
		}
		else if (status == 0)
			break ;
	if (status == -1)
		printf("Response receive failed: %s", strerror(errno));
	*result = response;
	return (status == -1 ? -1 : 0);
}

int			parse_response_status(char *response)
{
	json_object		*object;
	json_object		*value;
	int				status;

	status = -1;
	object = json_tokener_parse(response);
	if (json_object_object_get_ex(object, "status", &value))
		status = json_object_get_int(value);
	json_object_put(value);
	json_object_put(object);
	return (status);
}

int						tm_status(const char **args)
{
	char	*request;
	char	*response;
	int		status;

	response = NULL;
	if (args && args[0] && strcmp(args[0], "--help") == 0)
		printf("Usage goes here\n");
	else
	{
		request = pack_to_json_str(REQUEST_STATUS, args);
		if (try_sending_all(request) != 0)
			dprintf(2, "Request sending failed with error: %s\n", strerror(errno));
		else
			try_receive_response(&response);
		if ((status = parse_response_status(response) == 0))
			printf("Hello, nice to reply with 0");
		free((void *)response);
		free((void *)request);
		return (status);
	}
	return (0);
}
