/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/04 13:03:25 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/04 14:08:49 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_common.h"

#define LOGNAME_FORMAT "taskmasterd.%d.log"
#define LOG_ENTRY_FORMAT "[%s] %s"

pthread_mutex_t		g_logfile_mutex;

static struct
{
	FILE		*logfile;
}				g_logger;

enum		e_log_level
{
	LOG_OFF   = 1,
	LOG_INFO  = 2,
	LOG_WARN  = 4,
	LOG_ERROR = 8,
	LOG_FATAL = 16,
	LOG_DEBUG = 32
};

static const char * const	g_log_levels[] =
{
	"OFF",
	"INFO",
	"WARN",
	"ERROR",
	"FATAL",
	"DEBUG"
};

int			logger_init(const char *app_name)
{
	char			buf[1025];
	char			*env;
	char			*time_str;
	time_t			time;
	struct tm		time_alt;

	bzero(buf, sizeof(buf));
	if ((env = getenv("TASKMASTER_LOGFILE")))
		g_logger.logfile = fopen(env, "a+");
	else
	{
		snprintf(buf, 1024, LOGNAME_FORMAT, (int)getpid());
		g_logger.logfile = fopen(buf, "a+");
	}
	if (g_logger.logfile == NULL)
	{
		dprintf(2, "Failed to create logfile stream: %s", strerror(errno));
		return (-1);
	}
	fseek(g_logger.logfile, 0L, SEEK_END);
	pthread_mutex_init(&g_logfile_mutex, NULL);
	fprintf(g_logger.logfile, "%s logger initialized, ", app_name);
	return (0);
}
