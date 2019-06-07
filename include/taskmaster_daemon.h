/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   taskmaster_daemon.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/20 12:13:30 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/07 17:35:05 by obamzuro         ###   ########.fr       */
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
# include <pthread.h>

# include <errno.h>

//# include <sys/syslog.h>

# include <sys/resource.h>
# include <sys/types.h>

# include <sys/socket.h>
# include <sys/un.h>

# include <netdb.h>
# include <netinet/in.h>

# include <sys/times.h>
# include <limits.h>

# include "libft.h"
# include "taskmaster_common.h"

# ifdef __linux__
#  include <wait.h>
# endif

# ifdef TASKMASTER_CLI_H
#  error You should not include taskmaster_cli.h with taskmaster_daemon.h
# endif

#ifndef THREAD_POOL_CAPACITY
# define THREAD_POOL_CAPACITY 64
#endif

struct				s_thread_pool
{
	pthread_t		thread[THREAD_POOL_CAPACITY];
	struct s_packet	*packet_queues[THREAD_POOL_CAPACITY];
	void			*args[THREAD_POOL_CAPACITY];
	bool			is_busy[THREAD_POOL_CAPACITY];
	int				threads_number;
	int				pool_capacity;
};

struct				s_thread_args
{
	int			socket_fd;
	int			thread_id;
};

typedef struct	s_socket
{
	int					socket_domain;
	int					fd;
	union
	{
		struct sockaddr_un	local;
		struct sockaddr_in	inet;
	}					addr;
}				t_socket;

struct			s_master
{
	mode_t		tm_umask;
	t_socket	*local;
	t_socket	*inet;
	int			logfile;
};

/*
 * TODO: oi, this name doesn't correspond to The Norme
 */

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

typedef struct	s_ftvector
{
	void			**elem;
	unsigned int	capacity;
	unsigned int	len;
}				t_ftvector;

typedef enum	e_state
{
	UKNOWN = 0b1,
	STARTING = 0b10,
	RUNNING = 0b100,
	BACKOFF = 0b1000,
	EXITED = 0b10000,
	STOPPING = 0b100000,
	STOPPED = 0b1000000,
	FATAL = 0b10000000
}				t_state;

typedef struct	s_process
{
	pid_t		pid;
	t_state		state;
	int16_t		restart_attempts;
	time_t		time_begin;
	time_t		time_stop;
}				t_process;

typedef struct	s_job
{
//	int			jid;
//	pid_t		pid;
	t_process		*processes;
	unsigned int	processes_length;

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

struct			s_alteration
{
	pid_t				pid;
	char				*name;
	enum e_request		request;
	struct s_alteration	*next;
};

extern struct s_master		*g_master;

/*
** init
*/

struct s_socket		*socket_create_local(const char *filename);
struct s_socket		*socket_create_inet(const char *address);

int				read_fd(const int fd, char **result);
int				read_filename(const char *file, char **data);

/*
** Network interactions
*/
void __attribute__((noreturn))	*accept_pthread_loop(void *socket);

int				respond_status(const struct s_packet *packet);
int				respond_invalid(const struct s_packet *packet);
int				respond_stop(const struct s_packet *packet);

/*
** Network Thread Pool
*/
void				tpool_init(void);
int					tpool_create_thread(const pthread_attr_t *attr,
										void *(*runnable)(void *),
										void *arg);
void				tpool_finalize_thread(int id);
void				*tpool_arg(int socket_fd);

/*
** MY TERRITORY
*/

void		process_handling();

int				process_config(t_ftvector jobs);
void			init_ftvector(t_ftvector *vec);
void			free_ftvector(t_ftvector *vec);
void			push_ftvector(t_ftvector *vec, void *line);

void			d_restart();

extern t_ftvector			*g_jobs;
void			sigchld_handler(void);

void		d_stop(const unsigned int job_num, const unsigned int process_num);
void		alrm_handler(int signo __attribute__((unused)));
#endif
