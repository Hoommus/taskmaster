/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tm_banish.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/07 18:42:08 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/07 22:33:41 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"

int			tm_banish(const char **args)
{
	struct s_packet		*packet;

	packet = packet_create_json(json_object_new_object(), REQUEST_BANISH, NULL);
	net_send(g_shell->daemon.socket, packet);
	packet_free(&packet);
	net_get(g_shell->daemon.socket, &packet);
	return (args == NULL);
}
