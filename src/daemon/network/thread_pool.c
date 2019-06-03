/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread_pool.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/03 11:15:25 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/03 15:35:00 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"

/*
** It's not really a _pool_, although can have such capabilities
*/

static pthread_mutex_t		g_mutex_thread_pool;
static struct s_thread_pool	*g_thread_pool;

void						tpool_init(void)
{
	g_thread_pool = calloc(1, sizeof(struct s_thread_pool));
	g_thread_pool->pool_capacity = THREAD_POOL_CAPACITY;
	pthread_mutex_init(&g_mutex_thread_pool, NULL);
}

int							tpool_create_thread(const pthread_attr_t *attr,
												void *(*runnable)(void *),
												void *arg)
{
	int		i;
	int		status;

	i = -1;
	status = -1;
	pthread_mutex_lock(&g_mutex_thread_pool);
	while (++i < g_thread_pool->pool_capacity &&
		   g_thread_pool->threads_number < THREAD_POOL_CAPACITY)
		if (!g_thread_pool->is_busy[i])
		{
			((struct s_thread_args *)arg)->thread_id = i;
			if ((status =
				pthread_create(g_thread_pool->thread + i, attr, runnable, arg)))
				break ;
			g_thread_pool->args[i] = arg;
			g_thread_pool->is_busy[i] = true;
			g_thread_pool->threads_number++;
			status = 0;
			break ;
		}
	pthread_mutex_unlock(&g_mutex_thread_pool);
	return (status);
}

void						tpool_finalize_thread(int id)
{
	pthread_mutex_lock(&g_mutex_thread_pool);
	g_thread_pool->threads_number--;
	g_thread_pool->is_busy[id] = false;
	close(((struct s_thread_args *)g_thread_pool->args[id])->socket_fd);
	free(g_thread_pool->args[id]);
	g_thread_pool->args[id] = NULL;
	bzero(g_thread_pool->thread + id, sizeof(pthread_t));
	pthread_mutex_unlock(&g_mutex_thread_pool);
}

void						*tpool_arg(int socket_fd)
{
	struct s_thread_args	*arg;

	arg = calloc(1, sizeof(struct s_thread_args));
	arg->socket_fd = socket_fd;
	return (arg);
}
