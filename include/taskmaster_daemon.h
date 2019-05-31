/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   taskmaster_daemon.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/20 12:13:30 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/31 14:02:49 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef TASKMASTER_DAEMON_H
# define TASKMASTER_DAEMON_H

# include <fcntl.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include <stdio.h>
# include <signal.h>
# include <stdbool.h>
# include <sys/stat.h>

# include <errno.h>

# include <sys/resource.h>
# include <sys/types.h>

# include <sys/socket.h>
# include <sys/un.h>

# include <netdb.h>
# include <netinet/in.h>

# include "libft.h"
# include "taskmaster_common.h"

typedef struct	s_socket
{
	int					socket_domain;
	int					fd;
	union
	{
		struct sockaddr_un	unix;
		struct sockaddr_in	inet;
	}					addr;
}				t_socket;

struct			s_master
{
	mode_t		tm_umask;
	// First 4 sockets are unix, the rest are inet
	t_socket	*sockets[8];
	int			logfile;

};

typedef enum	e_policyType
{
	POLICY_RESTART_ALWAYS = 0b1,
	POLICY_RESTART_NEVER = 0b10,
	POLICY_RESTART_UNEXPECTED = 0b100,
	POLICY_REDIRECT_DISCARD = 0b1000,
	POLICY_REDIRECT_FILES = 0b10000,
	POLICY_START_ON_LAUNCH = 0b100000,
}				t_policyType;

# define ORIGIN_CONFIG 1
# define ORIGIN_CLIENT 2

/*
** All of these are signed to allow special value of -1
*/

//typedef struct	s_process
//{
//	pid_t		pid;
//	int			state;
//}				t_process;

typedef struct	s_job
{
//	int			jid;
	pid_t		pid;
	t_state		state;
//	t_process	*processes;
	char		**args;
	int64_t		policy;
	int64_t		program_duplicates;
	int64_t		successful_start_timeout;
	int16_t		restart_attempts;
	int64_t		graceful_stop_timeout;
	int8_t		graceful_stop_signal;
	int8_t		expected_statuses;
	int8_t		expected_statuses_len;
	char		**environ;
	char		*working_dir;
	mode_t		umask;

	char		*in;
	char		*out;
	char		*err;

	int8_t		origin;
}				t_job;

extern struct s_master		*g_master;

const t_socket	*register_socket(const t_socket *socket);

t_socket		*create_socket(int domain, const char *filename, const char *address);

int				read_fd(const int fd, char **result);
int				read_filename(const char *file, char **data);

/*
** MY TERRITORY
*/

extern t_ftvector			*g_jobs;

typedef struct	s_ftvector
{
	void	**elem;
	int		capacity;
	int		len;
}				t_ftvector;
int				process_config(t_ftvector jobs);
void			init_ftvector(t_ftvector *vec);
void			free_ftvector(t_ftvector *vec);
void			push_ftvector(t_ftvector *vec, void *line);

typedef enum	e_state
{
	UKNOWN = 0b1,
	STARTING = 0b10,
	RUNNING = 0b100,
	BACKOFF = 0b1000,
	EXITED = 0b10000,
	STOPPING = 0b100000,
	STOPPED = 0b1000000,
	DIED = 0b10000000
}				t_state;

void			d_restart();
#endif
