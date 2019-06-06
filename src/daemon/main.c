/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/20 12:10:31 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/04 16:48:49 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"
#define SOCKET_FILE "/var/tmp/taskmasterd.socket"

struct s_master		*g_master;
t_ftvector			*g_jobs;

// TODO: Use port 4242 for tcp
void	create_sockets(void)
{
	struct s_socket		*local;

	local = create_socket(AF_LOCAL, SOCKET_FILE, NULL);
	// TODO: consider limiting connections to 1 to avoid session-like shenanigans in this project
	//         NOPE
	if (listen(local->fd, 8) == -1)
		dprintf(g_master->logfile, "Listening to socket failed: %s\n", strerror(errno));
	else
		dprintf(g_master->logfile, "Started listening socket successfully\n");
	g_master->local = local;
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

void	destructor(void)
{
	close(g_master->logfile);
	close(g_master->local->fd);
//	close(g_master->inet->fd);
	remove(SOCKET_FILE);
}

// TODO: use TASKMASTERD_CONFIG environment variable or argument for config file specification
// TODO: make it possible to configure daemon to run in foreground (why?..)
// TODO: show usage at standard error if no config file specified in any way

// TODO: Don't forget to close sockets inside fork

int		main(int argc __attribute__((unused)), char **argv __attribute__((unused)))
{
	sigset_t	sigset;
	sigset_t	osigset;
	int			err;
	int			signo;

	g_master = calloc(1, sizeof(struct s_master));
	g_master->logfile = open("/tmp/hello_world.socket.log", O_CREAT | O_APPEND | O_RDWR, 0644);
	dprintf(g_master->logfile, "Initialising daemon...\n");
	create_daemon();
	dprintf(g_master->logfile, "Creating sockets...\n");
	create_sockets();
	tpool_init();

	g_jobs = (t_ftvector *)malloc(sizeof(t_ftvector));

	sigemptyset(&sigset);
	sigaddset(&sigset, SIGCHLD);
	if ((err = pthread_sigmask(SIG_BLOCK, &sigset, &osigset)))
		dprintf(g_master->logfile, "Pthread_sigmask error\n%s\n", strerror(err));

	process_handling();

//	tpool_create_thread(NULL, accept_pthread_loop, &g_master->local->fd);
//	tpool_create_thread(NULL, accept_pthread_loop, &g_master->inet->fd);
	atexit(destructor);
	while (ponies_teleported())
	{
		if ((err = sigwait(&sigset, &signo)))
			// TODO exit or smth smart
			dprintf(g_master->logfile, "Sigwait error\n%s\n", strerror(err));
		else if (signo == SIGCHLD)
		{
			dprintf(g_master->logfile, "SIGCHLD signal!\n");
			sigchld_handler(signo);
			d_restart();
		}
		dprintf(g_master->logfile, "signal %d!\n", signo);
	}
	return (0);
}
