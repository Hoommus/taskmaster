/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_handling.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/23 17:58:26 by obamzuro          #+#    #+#             */
/*   Updated: 2019/06/06 22:09:26 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"

char		*argsHardcoded[] =
{
	"/bin/bash", "src/daemon/echoer.sh", NULL
//	"/usr/bin/base64", "/dev/urandom", NULL
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
	hardjob->processes = (t_process *)malloc(1 * sizeof(t_process));
	hardjob->processes_length = 1;
	// ???? really need?
	hardjob->processes[0].pid = 0;
	hardjob->processes[0].state = STARTING;
	// TODO copy copy copy?
	hardjob->processes[0].restart_attempts = 1;
	// ????
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
	hardjob->out = "./1234"; hardjob->err = NULL;

	hardjob->origin = ORIGIN_CONFIG;
	init_ftvector(g_jobs);
	push_ftvector(g_jobs, hardjob);
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

void		sigchld_handler_process_handle(	int statloc,
											time_t *current_time,
											t_job *current_job,
											t_process *current_process)
{
	// if process is successfully started it's state will be
	// STARTING for time while it doesn't receive SIGCHLD sig
	// and change to EXITED -> RUNNING
	if (current_process->state == STOPPED)
		return ;
	else if (current_process->state == STARTING &&
			*current_time - current_process->time_begin <
			current_job->successful_start_timeout)
	{
		if (current_process->restart_attempts)
		{
			current_process->state = BACKOFF;
			--current_process->restart_attempts;
		}
		else
			current_process->state = FATAL;
	}
	else if (current_process->state | RUNNING | STARTING &&
			(current_job->policy & POLICY_RESTART_ALWAYS ||
		(current_job->policy & POLICY_RESTART_UNEXPECTED
		 && statloc == current_job->expected_statuses)))
	{
		//dprintf(g_master->sockets[0]->fd, "%ld", current_time - job->time_begin);
		current_process->state = EXITED;
	}
}

void		sigchld_handler_find_process(	int statloc,
											time_t *current_time,
											pid_t *pid)
{
	unsigned int	job_num;
	unsigned int	process_num;
	t_job			*current_job;
	t_process		*current_process;

	job_num = -1;
	while (++job_num < g_jobs->len)
	{
		current_job = (t_job *)g_jobs->elem[job_num];
		process_num = -1;
		while (++process_num < current_job->processes_length)
		{
			current_process = &current_job->processes[process_num];
			if (current_process->pid == *pid)
			{
				sigchld_handler_process_handle(statloc, current_time,
						current_job, current_process);
				break ;
			}
		}
	}
}

void		sigchld_handler(void)
{
	pid_t	pid;
	int		statloc;
	time_t	current_time;

	statloc = 0;
	pid = wait(&statloc);
	if (WIFEXITED(statloc) && WEXITSTATUS(statloc) == 228)
		return ;
	current_time = time(NULL);
	sigchld_handler_find_process(statloc, &current_time, &pid);
}

void		d_stop(const unsigned int job_num, const unsigned int process_num)
{
	t_job			*current_job;
	t_process		*current_process;
	unsigned int	ret;

	//TODO block sigalarm
	current_job = (t_job *)g_jobs->elem[job_num];
	current_process = &current_job->processes[process_num];
	current_process->state = STOPPING;
	time(&current_process->time_stop);
	ret = alarm(current_job->graceful_stop_timeout);
	if (ret && ret < current_job->graceful_stop_timeout)
		alarm(current_job->graceful_stop_timeout);
}

void		d_start(const int job_num, const int process_num)
{
	pid_t		process;
	char		**envp;
	t_job		*current_job;
	t_process	*current_process;
//	struct tms	process_time;

	current_job = (t_job *)g_jobs->elem[job_num];
	current_process = &current_job->processes[process_num];
	log_fwrite(LOG_DEBUG, "Fork for %d job, %d process", job_num, process_num);
	// TODO: handle overflow time limit
	current_process->time_begin = time(NULL);
	current_process->state = STARTING;
	process = fork();
	if (process == -1)
		log_fwrite(LOG_ERROR, "Failed to fork %d job, %d process", job_num, process_num);
	else if (process == 0)
	{
		// TODO one create_new_env for job
		envp = create_new_env(current_job);
		chdir(current_job->working_dir);
		umask(current_job->umask);
		if (handle_redirections(current_job) == -1)
		{
			log_fwrite(LOG_DEBUG, "redirections failed!\n%s\n", strerror(errno));
			exit(228);
		}
		else
			log_fwrite(LOG_DEBUG, "redirections successed!\n");
		if (execve(current_job->args[0], current_job->args, envp) == -1)
		{
			log_fwrite(LOG_DEBUG, "exec failed!\n%s\n", strerror(errno));
			exit(228);
		}
	}
	else
	{
		current_process->pid = process;
	}
}

void		d_restart()
{
	unsigned int	job_num;
	unsigned int	process_num;
	t_job			*current_job;
	t_process		*current_process;

	job_num = -1;
	while (++job_num < g_jobs->len)
	{
		current_job = (t_job *)g_jobs->elem[job_num];
		process_num = -1;
		while (++process_num < current_job->processes_length)
		{
			current_process = &current_job->processes[process_num];
			if (current_process->state == EXITED)
			{
				log_fwrite(LOG_DEBUG, "RESTART %d job, %d process (%s) after exit",
						job_num, process_num, current_job->args[0]);
				d_start(job_num, process_num);
			}
			else if (current_process->state == FATAL)
				log_fwrite(LOG_DEBUG, "FATAL %d job, %d process (%s)",
						job_num, process_num, current_job->args[0]);
			else if (current_process->state == BACKOFF)
			{
				log_fwrite(LOG_DEBUG, "RESTART %d job, %d process (%s) after unsuccessfull run",
						job_num, process_num, current_job->args[0]);
				d_start(job_num, process_num);
			}
		}
	}
}

int			alrm_handler_stopper(		t_job *current_job,
										t_process *current_process,
										time_t now,
										int *mindif)
{
	int		dif;

	if (current_process->state == STOPPING)
	{
		dif = current_job->graceful_stop_timeout - (now - current_process->time_stop);
		if (dif > 0 && *mindif > dif)
			*mindif = dif;
		if (dif <= 0)
		{
			current_process->state = STOPPED;
			kill(current_process->pid, SIGKILL);
		}
	}
	return (*mindif);
}

void		alrm_handler_inner(time_t now, int mindif)
{
	unsigned int	job_num;
	unsigned int	process_num;
	t_job			*current_job;
	t_process		*current_process;

	job_num = -1;
	while (++job_num < g_jobs->len)
	{
		current_job = (t_job *)g_jobs->elem[job_num];
		process_num = -1;
		while (++process_num < current_job->processes_length)
		{
			current_process = &current_job->processes[process_num];
			mindif = alrm_handler_stopper(current_job, current_process, now, &mindif);
		}
	}
	if (mindif != INT_MAX)
		alarm(mindif);

}

void		alrm_handler(int signo __attribute__((unused)))
{
	time_t	now;
	int		mindif;

	time(&now);
	mindif = INT_MAX;
	alrm_handler_inner(now, mindif);
}

void		sigchld_wrapper(int signo __attribute__((unused))) {}

void		signal_attempting()
{
	struct sigaction	act;
//
	act.sa_handler = NULL;
	act.sa_handler = sigchld_wrapper;//igchld_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
#ifdef SA_INTERRUPT
	act.sa_flags |= SA_INTERRUPT;
#endif
	if (sigaction(SIGCHLD, &act, NULL) < 0)
		exit(123);
	act.sa_handler = alrm_handler;
//	act.sa_handler = NULL;
	act.sa_sigaction = NULL;
	sigemptyset(&act.sa_mask);
	// hz
	sigaddset(&act.sa_mask, SIGCHLD);
	act.sa_flags = 0;
	act.sa_flags |= SA_NODEFER;// | SA_SIGINFO;
	if (sigaction(SIGALRM, &act, NULL) < 0)
		exit (123);
	log_write(LOG_DEBUG, "Signals handled\n");
}

void		process_handling()
{
	unsigned int	job_num;
	unsigned int	process_num;
	t_job			*current_job;

	jobs_filler();
	signal_attempting();
	job_num = -1;
	while (++job_num < g_jobs->len)
	{
		current_job = (t_job *)g_jobs->elem[job_num];
		process_num = -1;
		while (++process_num < current_job->processes_length)
			d_start(job_num, process_num);
	}
}
