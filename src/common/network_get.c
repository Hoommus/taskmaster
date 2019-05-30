/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network_get.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/28 12:09:17 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/30 17:57:16 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"

#ifdef BUFF_SIZE
# undef BUFF_SIZE
# define BUFF_SIZE 1024
#endif
#define PACKET_DELIMITER '\4'
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
	else if (buf_len > 0)
	{
		*line = ft_strsub(*buf, 0, slash - *buf);
		swap = *buf;
		*buf = ft_strsub(*buf, slash - *buf + 1, strlen(*buf));
		free(swap);
		return (1);
	}
	else
	{
		ft_memdel((void **)buf);
		return (0);
	}
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
	buf[fd] = buf[fd] == NULL ? calloc(1, sizeof(char) * 1) : buf[fd];
	b = calloc(1, sizeof(char) * (BUFF_SIZE + 1));
	while (strchr(buf[fd], '\n') == 0 && (cat = read(fd, b, BUFF_SIZE)) != 0 && errno != EAGAIN)
	{
		if (fd < 0 || cat == -1 || b == 0)
			return (-1);
		swap = buf[fd];
		buf[fd] = ft_strjoin(buf[fd], b);
		ft_strdel(&swap);
		ft_strdel(&b);
		b = calloc(1, sizeof(char) * (BUFF_SIZE + 1));
	}
	ft_strdel(&b);
	return (magic(content, &(buf[fd]), strlen(buf[fd])));
}

/*
** Reads data from the socket packet-by-packet and adds them all into a queue
*/

// TODO: try to reconnect / reaccept, if it is even possible

int			net_get(const int socket)
{
	char			*swap;
	ssize_t			status;
	struct timeval	time;
	int				flags;

	errno = 0;
	swap = NULL;
	flags = fcntl(socket, F_GETFL, 0);
	fcntl(socket, F_SETFL, flags | O_NONBLOCK);
	while ((status = get_next_packet(socket, &swap)))
		if (status < 0 && (errno == EPIPE || errno == ENETDOWN ||
							errno == ENETUNREACH || errno == ENETRESET))
		{
			dprintf(2, SH ": unable to receive response from the server: %s\n", strerror(errno));
			break ;
		}
		else if (status > 0)
		{
			gettimeofday(&time, NULL);
			packet_enqueue(packet_create(swap, time));
		}
	fcntl(socket, F_SETFL, flags & (~O_NONBLOCK));
	dprintf(2, "\n");
	return (status == -1 ? -1 : 0);
}



