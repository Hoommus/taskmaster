/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   taskmaster_daemon.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/20 12:13:30 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/22 15:28:29 by vtarasiu         ###   ########.fr       */
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

# include <libft.h>

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

enum			e_policy
{
	POLICY_RESTART_ALWAYS,
	POLICY_RESTART_NEVER,
	POLICY_RESTART_UNEXPECTED,
	POLICY_REDIRECT_DISCARD,
	POLICY_REDIRECT_FILES,
	POLICY_START_ON_LAUNCH,
};

# define ORIGIN_CONFIG 1
# define ORIGIN_CLIENT 2

/*
** All of these are signed to allow special value of -1
*/

typedef struct	s_job
{
	pid_t		pid;
	char		**args;
	int64_t		policy;
	int64_t		job_duplicates;
	int64_t		successful_start_timeout;
	int16_t		restart_attempts;
	int64_t		graceful_stop_timeout;
	int8_t		graceful_stop_signal;
	int8_t		*expected_statuses;
	char		**environ;
	char		*working_dir;
	mode_t		umask;

	int8_t		origin;
}				t_job;

extern struct s_master		*g_master;

const t_socket	*register_socket(const t_socket *socket);

t_socket		*create_socket(int domain, const char *filename, const char *address);

int				read_fd(const int fd, char **result);
int				read_filename(const char *file, char **data);


#endif