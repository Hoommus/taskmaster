/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   taskmaster_common.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/24 12:40:25 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/30 17:54:23 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TASKMASTER_COMMON_H
# define TASKMASTER_COMMON_H

# include "json.h"
# include <time.h>
# include <sys/time.h>

# ifdef __linux__
#  undef unix
# endif

# define RESPONSE_TIMEOUT_USECONDS

enum				e_request
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

/*
** Common network interactions
*/

struct				s_packet
{
	bool			is_content_parsed;
	char			*content;
	json_object		*json_content;
	enum e_request	request;
	struct timeval	timestamp;
	struct s_packet	*next;
};

typedef int			(*t_resolver_fun)(const struct s_packet *packet);

struct				s_resolver
{
	enum e_request command;
	t_resolver_fun resolver;
};

extern struct s_resolver	g_resolvers[];

struct s_packet	*packet_create(const char *contents, struct timeval timestamp);
int				packet_enqueue(struct s_packet *packet);
int				packet_dequeue(struct s_packet *packet);
int				packet_destroy(struct s_packet **packet);
int				packet_resolve(struct s_packet *packet);
int				packet_resolve_all(void);
int				packet_resolve_first(enum e_request type);

int				net_send(int socket, const char *request);
int				net_get(int socket);

#endif
