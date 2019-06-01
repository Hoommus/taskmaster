/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_handling.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obamzuro <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/23 17:58:26 by obamzuro          #+#    #+#             */
/*   Updated: 2019/05/27 19:47:55 by obamzuro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"

char		*argsHardcoded[] =
{
	"base64", "/dev/random", NULL
};

int8_t		expected_statuses[] =
{ SIGINT, -1 };

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

int			jobs_filler(t_ftvector *jobs)
{
	t_job	hardjob;

	hardjob.jid = 0;
	hardjob.processes = NULL;
	hardjob.args = argsHardcoded;
	hardjob.policy = POLICY_RESTART_ALWAYS | POLICY_START_ON_LAUNCH;
	hardjob.job_duplicates = 0;
	hardjob.successful_start_timeout = 100;
	hardjob.restart_attempts = 1;
	hardjob.graceful_stop_timeout = 0;
	hardjob.graceful_stop_signal = SIGUSR1;
	hardjob.expected_statuses = expected_statuses;
	hardjob.environ = environHardcoded;
	hardjob.working_dir = "/";
	hardjob.umask = 0;
	hardjob.in = NULL;
	hardjob.out = "/tmp/123";
	hardjob.err = NULL;

	hardjob.origin = ORIGIN_CONFIG;
	init_ftvector(jobs);
	push_ftvector(jobs, &hardjob);
	return (0);
}

int			redirection_substitute(char *filename, __unused int where, __unused int flag)
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

void		process_handling(t_ftvector *jobs)
{
	pid_t		process;
	char		**envp;
	int			i;
	t_job		*currentjob;

	jobs_filler(jobs);
	i = 0;
	while (i < jobs->len)
	{
		process = fork();
		if (process == -1)
			dprintf(g_master->logfile, "Failed to fork %d job\n", i);
		else if (process == 0)
		{
			currentjob = (t_job *)jobs->elem[i];
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
}
