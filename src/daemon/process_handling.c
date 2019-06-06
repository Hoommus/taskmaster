/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_handling.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/23 17:58:26 by obamzuro          #+#    #+#             */
/*   Updated: 2019/06/04 16:48:46 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"

char		*argsHardcoded[] =
{
	"/bin/bash", "src/daemon/echoer.sh", NULL
};

int8_t		expected_statuses[] =
{ 0, 1 };

char		*environHardcoded[] =
{
	"key=value", NULL
};

char		**create_new_env(t_job *job)
{
	extern char		**environ;
	unsigned int	i;
	unsigned int	lenenv;
	unsigned int	lenjob;
	char			**newenviron;

	lenenv = 0;
	lenjob = 0;
	while (environ[lenenv])
		++lenenv;
	while (job->environ[lenjob])
		++lenjob;
	lenjob = lenjob ? lenjob : 1;
	newenviron = (char **)malloc((lenenv + lenjob + 1) * sizeof(char *));
	i = 0;
	while (i < lenenv)
	{
		newenviron[i] = strdup(environ[i]);
		++i;
	}
	while (i < lenenv + lenjob)
	{
		newenviron[i] = strdup(job->environ[i - lenenv]);
		++i;
	}
	newenviron[i] = NULL;
	return (newenviron);
}

int			jobs_filler()
{
	t_job	*hardjob;

	hardjob = (t_job *)malloc(sizeof(t_job));
	hardjob->pid = 0;
	hardjob->state = STARTING;
//	hardjob->processes = NULL;
	hardjob->args = argsHardcoded;
	hardjob->policy = POLICY_RESTART_ALWAYS | POLICY_START_ON_LAUNCH;
	hardjob->program_duplicates = 0;
	hardjob->successful_start_timeout = 100;
	hardjob->restart_attempts = 1;
	hardjob->graceful_stop_timeout = 10;
	hardjob->graceful_stop_signal = SIGTSTP;
	hardjob->expected_statuses = 0;
	hardjob->expected_statuses_len = 2;
	hardjob->environ = environHardcoded;
	hardjob->working_dir = "./";
	hardjob->umask = 0;
	hardjob->in = NULL;
	hardjob->out = "./1234";
	hardjob->err = NULL;

	hardjob->origin = ORIGIN_CONFIG;
	init_ftvector(g_jobs);
	push_ftvector(g_jobs, hardjob);
	dprintf(g_master->logfile, "Filler for %d job\n", 0);
	return (0);
}

int			redirection_substitute(char *filename, __unused int where, __unused int flag)
{
	int		fd;

	if (filename)
	{
		if ((fd = open(filename, flag, 0777)) != -1)
		{
			dup2(fd, where);
			close(fd);
		}
		else
			return (-1);
	}
	return (0);
}

int			handle_redirections(t_job *job)
{
	if (redirection_substitute(job->in, STDIN_FILENO, O_RDONLY))
		return (-1);
	if (redirection_substitute(job->out, STDOUT_FILENO, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND))
		return (-1);
	if (redirection_substitute(job->err, STDERR_FILENO, O_WRONLY | O_CREAT | O_TRUNC))
		return (-1);
	return (0);
}

void		sigchld_handler(int signo __attribute__((unused)))
{
	pid_t	pid;
	int		statloc;
	int		i;
	t_job	*job;
	time_t	current_time;

	statloc = 0;
	pid = wait(&statloc);
	current_time = time(NULL);
	// SET_FL - TODO
	if (WIFEXITED(statloc) && WEXITSTATUS(statloc) == 228)
		return ;
	i = 0;
	while (i < g_jobs->len)
	{
		job = (t_job *)g_jobs->elem[i];
		if (job->pid == pid)
		{
			// if process is successfully started it's state will be
			// STARTING for time while it doesn't receive SIGCHLD sig
			// and change to EXITED -> RUNNING
			if (job->state == STOPPED)
				return ;
			else if (job->state == STARTING &&
					current_time - job->time_begin < job->successful_start_timeout)
			{
				if (job->restart_attempts)
				{
					job->state = BACKOFF;
					--job->restart_attempts;
				}
				else
					job->state = FATAL;
			}
			else if (job->state | RUNNING | STARTING &&
					(job->policy & POLICY_RESTART_ALWAYS ||
				(job->policy & POLICY_RESTART_UNEXPECTED
				 && statloc == job->expected_statuses)))
			{
				//dprintf(g_master->sockets[0]->fd, "%ld", current_time - job->time_begin);
				job->state = EXITED;
			}
			break ;
		}
		++i;
	}
//	if (fcntl(g_master->local->fd, F_SETFL, O_NONBLOCK) < 0)
//		// nah printf - bad func
//		dprintf(g_master->logfile, "NAH\n");
}

void		d_stop(int iter)
{
	t_job			*job;
	unsigned int	ret;

	//TODO block sigalarm
	job = (t_job *)g_jobs->elem[iter];
	job->state = STOPPING;
	time(&job->time_stop);
	ret = alarm(job->graceful_stop_timeout);
	if (ret && ret < job->graceful_stop_timeout)
		alarm(job->graceful_stop_timeout);
}

void		d_start(int iter)
{
	pid_t		process;
	char		**envp;
	t_job		*currentjob;
//	struct tms	process_time;

	currentjob = (t_job *)g_jobs->elem[iter];
	dprintf(g_master->logfile, "Fork for %d job\n", iter);
	// TODO: handle overflow time limit
	currentjob->time_begin = time(NULL);
	currentjob->state = STARTING;
	process = fork();
	if (process == -1)
		dprintf(g_master->logfile, "Failed to fork %d job\n", iter);
	else if (process == 0)
	{
		envp = create_new_env(currentjob);
		chdir(currentjob->working_dir);
		umask(currentjob->umask);
		if (handle_redirections(currentjob) == -1)
		{
			dprintf(g_master->logfile, "redirections failed!\n%s\n", strerror(errno));
			exit(228);
		}
		else
			dprintf(g_master->logfile, "redirections successed!\n");
		if (execve(currentjob->args[0], currentjob->args, envp) == -1)
		{
			dprintf(g_master->logfile, "exec failed!\n%s\n", strerror(errno));
			exit(228);
		}
	}
	else
	{
		currentjob->pid = process;
	}

}

void		d_restart()
{
	int		i;
	t_job	*job;
	// TODO: these sigset_t differ on Linux, so you should use sigaddset() functions
	//  to manipulate the mask. At the moment, this code won't compile on Linux

	i = 0;
//	sigemptyset(&sigset);
//	sigset |= SIGCHLD;
//	sigprocmask(SIG_BLOCK, &sigset, &osigset);
	//TODO check return
//	val = fcntl(g_master->local->fd, F_GETFL, 0);
//	val &= ~O_NONBLOCK;
//	ret = fcntl(g_master->local->fd, F_SETFL, val);
	while (i < g_jobs->len)
	{
		job = (t_job *)g_jobs->elem[i];
		if (job->state == EXITED)
		{
			dprintf(g_master->logfile, "RESTART %d job(%s) after exit\n", i, job->args[0]);
			d_start(i);
		}
		else if (job->state == FATAL)
			dprintf(g_master->logfile, "FATAL %d job(%s)\n", i, job->args[0]);
		else if (job->state == BACKOFF)
		{
			dprintf(g_master->logfile, "RESTART %d job(%s) after unsuccessfull run\n", i, job->args[0]);
			d_start(i);
		}
		++i;
	}
//	sigprocmask(SIG_SETMASK, &osigset, NULL);
}

void		alrm_handler(int signo __attribute__((unused)))
{
	int		i;
	t_job	*job;
	time_t	now;
	int		mindif;
	int		dif;

	i = 0;
	time(&now);
	mindif = INT_MAX;
	while (i < g_jobs->len)
	{
		job = (t_job *)g_jobs->elem[i];
		if (job->state == STOPPING)
		{
			dif = job->graceful_stop_timeout - (now - job->time_stop);
			if (dif > 0 && mindif > dif)
				mindif = dif;
			if (dif <= 0)
			{
				job->state = STOPPED;
				kill(job->pid, SIGKILL);
			}
		}
		++i;
	}
	if (mindif != INT_MAX)
		alarm(mindif);
}

void		signal_attempting()
{
//	struct sigaction	act;
//
//	act.sa_handler = NULL;
//	act.sa_sigaction = sigchld_handler;
//	sigemptyset(&act.sa_mask);
//	act.sa_flags = 0;
//#ifdef SA_INTERRUPT
//	act.sa_flags |= SA_INTERRUPT;
//#endif
//	if (sigaction(SIGCHLD, &act, NULL) < 0)
//		exit(123);
//	act.sa_handler = alrm_handler;
////	act.sa_handler = NULL;
//	act.sa_sigaction = NULL;
//	sigemptyset(&act.sa_mask);
//	// hz
//	sigaddset(&act.sa_mask, SIGCHLD);
//	act.sa_flags = 0;
//	act.sa_flags |= SA_NODEFER;// | SA_SIGINFO;
//	if (sigaction(SIGALRM, &act, NULL) < 0)
//		exit (123);
//	dprintf(g_master->logfile, "Signals handled\n");
}

void		process_handling()
{
	int			i;

	jobs_filler();
//	signal_attempting();
	i = -1;
	while (++i < g_jobs->len)
		d_start(i);
	dprintf(g_master->logfile, "process handled\n");
}
