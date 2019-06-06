/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/20 12:10:31 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/06 14:08:43 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"
#include <getopt.h>

#define SOCKET_FILE "/var/tmp/taskmasterd.socket"
#define CONFIG_DEFAULT "taskmasterconf.json"

struct s_master		*g_master;
t_ftvector			*g_jobs;

// TODO: Use port 4242 for tcp
int		create_sockets(void)
{
	int			status;

	g_master->local = socket_create_local(SOCKET_FILE);
	status = 0 - (g_master->local == 0);
	if (status == 0)
	{
		if (listen(g_master->local->fd, 8) == -1)
			log_fwrite(TLOG_INFO, "Failed to listen to a local socket: %s", strerror(errno));
		else
			log_write(TLOG_INFO, "Successfully started listening to a local socket");
	}
//	if (!socket_create_inet("123.456.789.123:4242"))
//		status -= 1;
	return (status);
}

pid_t	create_daemon(void)
{
//	struct rlimit	limits;
	const sigset_t	mask = {0};
	pid_t			pid;
	int32_t			null;
	u_int64_t		i;

	if (fork() > 0)
		exit(EXIT_SUCCESS);
	log_write(TLOG_DEBUG, "Successfully forked first time");
	i = 2;
//	if (getrlimit(RLIMIT_NOFILE, &limits) == 0)
//		while (++i < limits.rlim_cur)
//			if ((int)i != fileno(logger_get_file()))
//				close(i);
	log_write(TLOG_DEBUG, "Closed all redundant fds");
	sigprocmask(SIG_SETMASK, &mask, NULL);
	setsid();
	umask(0);
	log_write(TLOG_DEBUG, "Set sigprocmask, sid and umask(0)");
	if ((pid = fork()) > 0)
		exit(EXIT_SUCCESS);
	else if (pid == -1)
	{
		log_write(TLOG_FATAL, "Failed to fork second time");
		exit(2);
	}
	log_write(TLOG_DEBUG, "Successfully forked second time, now we're real daemon");
	null = open("/dev/null", O_RDWR);
	dup2(0, null);
	dup2(1, null);
	dup2(2, null);
	close(null);
	log_write(TLOG_DEBUG, "Replaced all standard fds with /dev/null");
	return (pid);
}

void	destructor(void)
{
	fclose(logger_get_file());
//	close(g_master->logfile);
	close(g_master->local->fd);
//	close(g_master->inet->fd);
	remove(SOCKET_FILE);
}

struct		s_args
{
	char				*config_file;
	char				*logfile;
	enum e_log_level	log_level;
};

char				*file_check(char *path)
{
	struct stat	s;

	if (path == NULL)
		return (NULL);
	bzero(&s, sizeof(struct stat));
	if (stat(path, &s) == -1)
		dprintf(2, "%s: %s", strerror(errno), path);
	else if (!S_ISREG(s.st_mode))
		dprintf(2, "%s is not a regular file\n", path);
	else if ((s.st_mode & S_IRUSR) == 0)
		dprintf(2, "permission denied: %s\n", path);
	else
		return (path);
	return (NULL);
}

static void			usage(void)
{
	dprintf(2, "huh, usage\n");
	exit(0);
}

struct s_args		parse_args(int argc, char **argv)
{
	int								opt;
	static const struct option		longopts[] = {
		{ "loglevel", required_argument, NULL, 1},
		{ "logfile", required_argument, NULL, 2},
		{ "config", required_argument, NULL, 'c'},
		{ "help", optional_argument, NULL, 'h'},
		{ NULL, 0, NULL, 0}
	};
	static struct s_args			args;

	args.config_file = CONFIG_DEFAULT;
	args.log_level = TLOG_DEBUG; // TODO: change this to TLOG_WARN
	while ((opt = getopt_long(argc, argv, "c:h:", longopts, NULL)) != -1)
	{
		if (opt == 1)
			args.log_level = logger_level_parse(optarg);
		else if (opt == 2)
			args.logfile = optarg;
		else if (opt == 'c')
			args.config_file = file_check(optarg);
		else if (opt == 'h')
			usage();
	}
	return (args);
}

// TODO: use TASKMASTERD_CONFIG environment variable or argument for config file specification
// TODO: make it possible to configure daemon to run in foreground (why?..)
// TODO: show usage at standard error if no config file specified in any way

// TODO: Don't forget to close sockets inside fork

int					main(int argc, char **argv)
{
	sigset_t			sigset;
	sigset_t			osigset;
	struct s_args		args;

	remove(SOCKET_FILE);
	args = parse_args(argc - 1, argv + 1);
	if (args.config_file == NULL)
		exit((dprintf(2, "specify a valid path to a config file\n") & 0) | 2);
	if (args.log_level == (enum e_log_level)-1)
		dprintf(2, "loglevel has unknown level, setting to INFO\n");
	logger_init(args.log_level, argv[0]);
	g_master = calloc(1, sizeof(struct s_master));
	log_write(TLOG_DEBUG, "Initialising daemon");
	create_daemon();
	tpool_init();
	log_write(TLOG_DEBUG, "Creating sockets");
	if (create_sockets() == -1)
		log_write(TLOG_ERROR, "Failed to create some sockets");
	else
		tpool_create_thread(NULL, accept_pthread_loop, tpool_arg(g_master->local->fd));

	g_jobs = (t_ftvector *)malloc(sizeof(t_ftvector));

	sigemptyset(&sigset);
	sigset |= SIGCHLD;
	sigprocmask(SIG_BLOCK, &sigset, &osigset);
	process_handling();
//	sigprocmask(SIG_SETMASK, &osigset, NULL);
	atexit(destructor);
	while (ponies_teleported())
	{
		if (sigsuspend(&osigset) != -1)
			log_write(TLOG_ERROR, "SIGSUSPEND error\n");
		d_restart();
//		pause();
//		if (errno == EAGAIN)
//		{
//			log_write(TLOG_ERROR, "EAGAIN connection\n%s\n", strerror(errno));
//		}
//		else if (errno == EINTR)
//			log_write(TLOG_ERROR, "EINTR connection\n%s\n", strerror(errno));
//		else
//			log_write(TLOG_ERROR, "Connection acceptance failed\n%s\n", strerror(errno));
	}
	return (0);
}
