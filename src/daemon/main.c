/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/20 12:10:31 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/01 18:55:52 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"
#define SOCKET_FILE "/var/tmp/taskmasterd.socket"


struct s_master		*g_master;
t_ftvector			*g_jobs;

// TODO: Use port 4242 for tcp
void	create_sockets(void)
{
	struct s_socket		*unix;

	unix = create_socket(AF_LOCAL, SOCKET_FILE, NULL);
	// TODO: consider limiting connections to 1 to avoid session-like shenanigans in this project
	if (listen(unix->fd, 8) == -1)
		dprintf(g_master->logfile, "Listening to %d failed\n", unix->fd);
	else
		dprintf(g_master->logfile, "Started listening socket successfully\n");
	g_master->sockets[0] = unix;
}

pid_t	create_daemon(void)
{
	struct rlimit	limits;
	const sigset_t	mask = {0};
	pid_t			pid;
	int32_t			null;
	u_int64_t		i;

	if (fork() > 0)
		exit(EXIT_SUCCESS);
	dprintf(g_master->logfile, "Successfully forked first time.\n");
	i = 2;
	if (getrlimit(RLIMIT_NOFILE, &limits) == 0)
		while (++i < limits.rlim_cur)
			if ((int)i != g_master->logfile)
				close(i);
	dprintf(g_master->logfile, "Closed all redundant fds.\n");
	sigprocmask(SIG_SETMASK, &mask, NULL);
	setsid();
	umask(0);
	dprintf(g_master->logfile, "Set sigprocmask, sid and umask(0).\n"); if ((pid = fork()) > 0) exit(EXIT_SUCCESS);
	dprintf(g_master->logfile, "Successfully forked second time, now we're real daemon.\n");
	null = open("/dev/null", O_RDWR);
	dup2(0, null);
	dup2(1, null);
	dup2(2, null);
	close(null);
	dprintf(g_master->logfile, "Replaced all standard fds with /dev/null.\n");
	return (pid);
}

// TODO: use TASKMASTERD_CONFIG environment variable or argument for config file specification
// TODO: make it possible to configure daemon to run in foreground (why?..)
// TODO: show usage at standard error if no config file specified in any way

// TODO: Don't forget to close sockets inside fork

int		main(int argc __attribute__((unused)), char **argv __attribute__((unused)))
{
	struct sockaddr_storage		client;
	int							connection;
	u_int32_t					client_size;
//	char						buffer[1024];

	remove(SOCKET_FILE);
	g_master = calloc(1, sizeof(struct s_master));
	g_master->logfile = open("/tmp/hello_world.socket.log", O_CREAT | O_APPEND | O_RDWR, 0644);
	dprintf(g_master->logfile, "Initialising daemon...\n");
	create_daemon();
	dprintf(g_master->logfile, "Creating sockets...\n");
	create_sockets();

	g_jobs = (t_ftvector *)malloc(sizeof(t_ftvector));
	process_handling();
	while ((connection = accept(g_master->sockets[0]->fd,
		(struct sockaddr *)&client, &client_size)) == -1)
	{
		if (errno == EAGAIN)
		{
			dprintf(g_master->logfile, "EAGAIN connection\n%s\n", strerror(errno));
			d_restart();
		}
		else if (errno == EINTR)
			dprintf(g_master->logfile, "EINTR connection\n%s\n", strerror(errno));
		else
			dprintf(g_master->logfile, "Connection acceptance failed\n%s\n", strerror(errno));
	}
//
//	accept_receive_respond_loop();
	return (0);
}
