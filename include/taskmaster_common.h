/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   taskmaster_common.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/24 12:40:25 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/24 13:55:11 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TASKMASTER_COMMON_H
# define TASKMASTER_COMMON_H

# include "json.h"

# ifdef __linux__
#  undef unix
# endif

# define RESPONSE_TIMEOUT_USECONDS

enum			e_request
{
	REQUEST_HEARTBEAT    = 0,
	REQUEST_STATUS       = 1,
	REQUEST_STOP         = 2,
	REQUEST_START        = 4,
	REQUEST_RESTART      = 8,
	REQUEST_RESET        = 16,
	REQUEST_BANISH       = 32,
	REQUEST_SUMMON       = 64,

};

#endif
