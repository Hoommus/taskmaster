/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serve_banish.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/06 17:34:45 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/06 17:36:32 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"

int							respond_banish(const struct s_packet *packet)
{

	log_write(TLOG_FATAL, "Client requested daemon banish");
	return (packet == NULL);
}