/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_auxiliary.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/21 19:31:05 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/21 19:31:05 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"

int				read_fd(const int fd, char **result)
{
	char		buffer[1024];
	char		*data;
	char		*swap;
	ssize_t		status;

	memset(buffer, 0, 1024);
	data = (char *)calloc(1, sizeof(char));
	while ((status = read(fd, buffer, 1023)))
	{
		if (status == -1)
		{
			free(data);
			*result = (char *)calloc(1, sizeof(char));
			return (-1);
		}
		swap = ft_strjoin(data, buffer);
		free(data);
		data = swap;
		memset(buffer, 0, 1024);
	}
	*result = data;
	return (1);
}

// TODO: replace error messages with logging
int				read_filename(const char *file, char **data)
{
	int			fd;
	struct stat	s;

	if (file == NULL)
		return (read_fd(STDIN_FILENO, data) & 0);
	else if (stat(file, &s) != 0)
	{
		dprintf(2, "taskmaster: %s: does not exist.\n", file);
		return (1);
	}
	else if (!S_ISREG(s.st_mode))
	{
		dprintf(2, "taskmaster: %s: ain't a regular file.\n", file);
		return (1);
	}
	fd = open(file, O_RDONLY);
	if (fd < 0)
	{
		dprintf(2, "taskmaster: %s: permission denied.\n", file);
		return (1);
	}
	read_fd(fd, data);
	close(fd);
	return (0);
}
