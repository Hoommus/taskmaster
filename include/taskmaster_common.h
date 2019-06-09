/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   taskmaster_common.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/24 12:40:25 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/08 10:31:38 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TASKMASTER_COMMON_H
# define TASKMASTER_COMMON_H

# include <stdio.h>

# include <stdbool.h>
# include <string.h>
# include <time.h>

# include <sys/time.h>
# include <sys/socket.h>

# include <sys/un.h>
# include <netdb.h>
# include <netinet/in.h>

# include <errno.h>

# include <fcntl.h>
# include <poll.h>

# include <pthread.h>

# include "json.h"
# include "libft.h"

# ifdef __linux__
#  ifndef  __unused
#   define __unused __attribute__((unused))
#  endif
#  undef unix
# endif

# define PACKET_DELIMITER 23
# define HANDSHAKE_REQUEST "Hello, nice to meet you."
# define HANDSHAKE_RESPONSE "Nice to meet you too."

# define RESPONSE_TIMEOUT_USECONDS

enum					e_request
{
	REQUEST_HANDSHAKE    = 0,
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

struct					s_packet
{
	int				respond_to;
	bool			is_content_parsed;
	char			*content;
	json_object		*json_content;
	enum e_request	request;
	struct timeval	timestamp;
	struct s_packet	*next;
};

typedef int				(*t_resolver_fun)(const struct s_packet *packet);

struct					s_resolver
{
	enum e_request	command;
	t_resolver_fun	resolver;
};

enum					e_remote
{
	REMOTE_UNIX,
	REMOTE_INET,
	REMOTE_CLIENT,
	REMOTE_NOT_APPLICABLE
};

typedef struct			s_remote_alt
{
	bool				is_alive;
	enum e_remote		type;
	int					sock_stream_fd;
	union
	{
		struct sockaddr_un			unix;
		struct sockaddr_in			inet;
		struct sockaddr_storage		client;
	}					addr;
}						t_remote_alt;


extern struct s_resolver	g_resolvers[];

struct s_packet	*packet_create(int socket, const char *contents, struct timeval timestamp);
struct s_packet	*packet_create_json(json_object *content, enum e_request request, const struct timeval *timestamp);
int				packet_enqueue(struct s_packet **queue_head, struct s_packet *packet);
int				packet_dequeue(struct s_packet *packet);
int				packet_free(struct s_packet **packet);
int				packet_resolve(struct s_packet *packet);
int				packet_resolve_all(struct s_packet *queue);
int				packet_resolve_first(enum e_request type);

int				net_send(int socket, struct s_packet *packet);
int				net_get(const int socket, struct s_packet **queue_head);

/*
** Logger
*/

enum				e_log_level
{
	TLOG_OFF = 0,
	TLOG_FATAL = 1,
	TLOG_SEVERE = 2,
	TLOG_ERROR = 4,
	TLOG_WARN = 8,
	TLOG_INFO = 16,
	TLOG_DEBUG = 128,
};

int					logger_init(enum e_log_level level, const char *app_name);
enum e_log_level	logger_level_parse(const char *str);
FILE				*logger_get_file(void);
int					log_write(enum e_log_level level, const char *message);
int __attribute__((__format__ (__printf__, 2, 3)))
				log_fwrite(enum e_log_level level, const char *format, ...);

/*
** Pony API
*/
ssize_t					ponies_teleported(void);

#endif
