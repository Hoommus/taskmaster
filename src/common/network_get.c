/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network_get.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/28 12:09:17 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/01 20:10:07 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"

#ifdef BUFF_SIZE
# undef BUFF_SIZE
# define BUFF_SIZE 1024
#endif
#define PACKET_DELIMITER 23
#define BUFFER_MAXIMUM_SIZE 2147483647 // It's 2 GiB

static int	magic(char **line, char **buf, size_t buf_len)
{
	char	*swap;
	char	*slash;

	if (buf_len > BUFFER_MAXIMUM_SIZE)
	{
		ft_memdel((void **)buf);
		return (-2);
	}
	if ((slash = strchr(*buf, PACKET_DELIMITER)) == NULL && buf_len > 0)
	{
		*line = strdup(*buf);
		ft_memdel((void **)buf);
		return (1);
	}
	if (buf_len > 0)
	{
		*line = ft_strsub(*buf, 0, slash - *buf);
		swap = *buf;
		*buf = ft_strsub(*buf, slash - *buf + 1, strlen(*buf));
		free(swap);
		return (1);
	}
	ft_memdel((void **)buf);
	return (0);
}

// TODO: create mechanism for discarding any non-complete packets in case of
//  some fatal network error (maybe create third 'flags' argument)
int			get_next_packet(const int fd, char **content)
{
	static char	*buf[4864];
	char		*b;
	char		*swap;
	long long	cat;

	if (fd < 0 || fd >= 4864 || content == NULL)
		return (-1);
	buf[fd] = buf[fd] == NULL ? ft_strnew(0) : buf[fd];
	b = calloc(1, sizeof(char) * (BUFF_SIZE + 1));
	while (strchr(buf[fd], PACKET_DELIMITER) == 0 && (cat = read(fd, b, BUFF_SIZE)) != 0
	&& errno != EAGAIN)
	{
		if (fd < 0 || cat == -1 || b == 0)
			return (-1);
		swap = buf[fd];
		buf[fd] = ft_strjoin(buf[fd], b);
		ft_strdel(&swap);
		ft_strdel(&b);
		b = calloc(1, sizeof(char) * (BUFF_SIZE + 1));
		if (cat != BUFF_SIZE)
			break ;
	}
	ft_strdel(&b);
	return (magic(content, &(buf[fd]), strlen(buf[fd])));
}

/*
** Reads data from the socket packet-by-packet and adds them all into a queue
*/

// TODO: try to reconnect / reaccept, if it is even possible
// TODO: reject "old" packets or decide what to do with them
int			net_get(const int socket, struct s_packet **queue_head)
{
	char			*swap;
	ssize_t			status;
	struct timeval	time;

	errno = 0;
	swap = NULL;
	(status = get_next_packet(socket, &swap));
	if (status < 0 && (errno == EPIPE || errno == ENETDOWN ||
			errno == ENETUNREACH || errno == ENETRESET))
	{
		return (-2);
	}
	else if (status >= 0 && swap != NULL)
	{
		gettimeofday(&time, NULL);
		packet_enqueue(queue_head, packet_create(socket, swap, time));
		ft_strdel(&swap);
	}
	return (status);
}



