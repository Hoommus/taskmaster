/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_handling.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/23 17:58:26 by obamzuro          #+#    #+#             */
/*   Updated: 2019/05/31 14:08:38 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"

char		*argsHardcoded[] =
{
	"base64", "/dev/random", NULL
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
	newenviron = (char **)malloc((lenenv + lenjob) * sizeof(char *));
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
	return (newenviron);
}

int			jobs_filler()
{
	t_job	hardjob;

	hardjob.pid = 0;
	hardjob.state = RUNNING;
//	hardjob.processes = NULL;
	hardjob.args = argsHardcoded;
	hardjob.policy = POLICY_RESTART_ALWAYS | POLICY_START_ON_LAUNCH;
	hardjob.program_duplicates = 0;
	hardjob.successful_start_timeout = 100;
	hardjob.restart_attempts = 1;
	hardjob.graceful_stop_timeout = 10;
	hardjob.graceful_stop_signal = SIGTSTP;
	hardjob.expected_statuses = 0;
	hardjob.expected_statuses_len = 2;
	hardjob.environ = environHardcoded;
	hardjob.working_dir = "/";
	hardjob.umask = 0;
	hardjob.in = NULL;
	hardjob.out = "/tmp/123";
	hardjob.err = NULL;

	hardjob.origin = ORIGIN_CONFIG;
	init_ftvector(g_jobs);
	push_ftvector(g_jobs, &hardjob);
	return (0);
}

int			redirection_substitute(char *filename, int where, int flag)
{
	int		fd;

	if (filename)
	{
		if ((fd = open(filename, O_RDONLY)) != -1)
		{
			dup2(fd, STDIN_FILENO);
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
	if (redirection_substitute(job->out, STDOUT_FILENO, O_WRONLY))
		return (-1);
	if (redirection_substitute(job->err, STDERR_FILENO, O_WRONLY))
		return (-1);
	return (0);
}

void		sigchld_handler(int signo)
{
	pid_t	pid;
	int		statloc;
	int		i;
	t_job	*job;

	pid = wait(&statloc);
	// SET_FL - TODO
	if (fcntl(g_master->sockets[0]->fd, F_SETFL, O_NONBLOCK) < 0)
		// nah printf - bad func
		dprintf(g_master->logfile, "NAH\n");
	i = 0;
	while (i < g_jobs->len)
	{
		job = (t_job *)g_jobs->elem[i];
		if (job->pid == pid)
		{
			if (job->policy == POLICY_RESTART_ALWAYS ||
				(job->policy == POLICY_RESTART_UNEXPECTED
				 && statloc == job->expected_statuses)
			job->state = EXITED;
			break ;
		}
	}
}

void		d_restart()
{
	int		i;
	t_job	*job;

	i = 0;
	while (i < g_jobs->len)
	{
		job = (t_job *)g_jobs->elem[i];
		if (job->state == EXITED)
			d_start(i);
	}
}

void		signal_attempting()
{
	struct sigaction	act;

	act.sa_handler = sigchld_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
#ifdef SA_INTERRUPT
	act.sa_flags |= SA_INTERRUPT
#endif
	if (sigaction(SIGCHLD, &act, NULL) < 0)
		exit(123);
}

void		d_start(int iter)
{
	pid_t		process;
	char		**envp;
	t_job		*currentjob;

	process = fork();
	if (process == -1)
		dprintf(g_master->logfile, "Failed to fork %d job\n", i);
	else if (process == 0)
	{
		currentjob = (t_job *)g_jobs->elem[i];
		envp = create_new_env(currentjob);
		handle_redirections(currentjob);
		chdir(currentjob->working_dir);
		umask(currentjob->umask);
		execve(currentjob->args[0], currentjob->args, envp);
	}
	else
	{

	}
	++i;

}

void		process_handling()
{
	int			i;
	sigset_t	sigset;
	sigset_t	osigset;

	jobs_filler();
	signal_attempting();
	sigemptyset(&sigset);
	sigset |= SIGCHLD;
	sigprocmask(SIG_BLOCK, &sigset, &osigset);
	i = -1;
	while (++i < g_jobs->len)
		d_start(i);
	sigprocmask(SIG_SETMASK, &osigset, NULL);
}
