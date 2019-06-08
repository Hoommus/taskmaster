/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   auxiliary.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/08 10:01:14 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/08 10:31:38 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_common.h"

/*
** this functions should be called 5 times on one socket in one run
*/

int		is_socket_ready(int socket_fd)
{
	struct pollfd	pollfd;
	static int		retry = 0;
	static int		fd;
	static int		ms_timeout = 1000;

	if (fd != socket_fd)
	{
		fd = socket_fd;
		ms_timeout = 1000;
	}
	retry = retry == 5 || fd != socket_fd ? 0 : retry;
	pollfd = (struct pollfd){fd, POLLIN | POLLOUT, 0};
	if (poll(&pollfd, 1, ms_timeout) == 0 && (pollfd.revents & POLLOUT) &&
		(pollfd.revents & POLLIN))
	{
		retry++;
		ms_timeout <<= retry;
		return (0);
	}
	else
		return (1);
}
