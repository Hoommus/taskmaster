/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/04 13:03:25 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/05 20:18:33 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_common.h"

#define LOGNAME_FORMAT "taskmasterd.log"

static pthread_mutex_t	g_logfile_mutex;

static struct
{
	FILE				*logfile;
	enum e_log_level	level;
}					g_logger;

FILE					*logger_get_file(void)
{
	return (g_logger.logfile);
}

static const char		*logger_level_str(enum e_log_level level)
{
	static const char *const	levels[] = {
		"OFF", "INFO", "WARN", "ERROR", "FATAL", "DEBUG", "SEVERE"
	};

	if (level == LOG_INFO)
		return (levels[1]);
	if (level == LOG_WARN)
		return (levels[2]);
	if (level == LOG_ERROR)
		return (levels[3]);
	if (level == LOG_FATAL)
		return (levels[4]);
	if (level == LOG_DEBUG)
		return (levels[5]);
	if (level == LOG_SEVERE)
		return (levels[6]);
	return ("Are you serious? Logger is disabled.");
}

enum e_log_level		logger_level_parse(const char *str)
{
	char				*copy;
	enum e_log_level	level;

	copy = strdup(str);
	if (strcmp(copy, "OFF") == 0)
		level = (LOG_OFF);
	else if (strcmp(copy, "INFO") == 0)
		level = (LOG_INFO);
	else if (strcmp(copy, "WARN") == 0)
		level = (LOG_WARN);
	else if (strcmp(copy, "ERROR") == 0)
		level = (LOG_ERROR);
	else if (strcmp(copy, "FATAL") == 0)
		level = (LOG_FATAL);
	else if (strcmp(copy, "DEBUG") == 0)
		level = (LOG_DEBUG);
	else if (strcmp(copy, "SEVERE") == 0)
		level = (LOG_SEVERE);
	else if (strcmp(copy, "ALL") == 0)
		level = (LOG_DEBUG);
	else
		level = -1;
	free(copy);
	return (level);
}

// TODO: Fallback to default file, if specified one is inaccessible
int						logger_init(enum e_log_level level, const char *app_name)
{
	char			buf[1025];
	char			*env;

	bzero(buf, sizeof(buf));
	g_logger.level = level == (enum e_log_level)-1 ? LOG_WARN : level;
	if ((env = getenv("TASKMASTER_LOGFILE")))
		g_logger.logfile = fopen(env, "a+");
	else
	{
		//snprintf(buf, 1024, LOGNAME_FORMAT, (int)getpid());
		g_logger.logfile = fopen(LOGNAME_FORMAT, "a+");
	}
	if (g_logger.logfile == NULL)
	{
		dprintf(2, "Failed to create logfile stream: %s", strerror(errno));
		return (-1);
	}
	fseek(g_logger.logfile, 0L, SEEK_END);
	pthread_mutex_init(&g_logfile_mutex, NULL);
	log_fwrite(LOG_INFO, "logger for %s [%d] started", app_name, getpid());
	return (0);
}

int						log_write(enum e_log_level level, const char *message)
{
	return (log_fwrite(level, "%s", message));
}

int						log_fwrite(enum e_log_level level, const char *format, ...)
{
	time_t			tm;
	char			date_buf[1025];
	va_list			args;
	struct timeval	timeval;

	if (level > g_logger.level)
		return (-1);
	va_start(args, format);
	bzero(date_buf, sizeof(date_buf));
	pthread_mutex_lock(&g_logfile_mutex);
	time(&tm);
	gettimeofday(&timeval, NULL);
	strftime(date_buf, 1024, "%d.%m.%Y %T", localtime(&tm));
	fprintf(g_logger.logfile, "%-6s [%s.%-4d] ", logger_level_str(level),
			date_buf, timeval.tv_usec / 100);
	vfprintf(g_logger.logfile, format, args);
	fprintf(g_logger.logfile, "\n");
	fflush(g_logger.logfile);
	pthread_mutex_unlock(&g_logfile_mutex);
	va_end(args);
	return (0);
}

