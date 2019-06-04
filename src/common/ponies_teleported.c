/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ponies_teleported.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/03 16:57:01 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/03 16:57:01 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_common.h"

/*
** Function reports how many ponies has been teleported
*/

ssize_t				ponies_teleported(void)
{
	ssize_t			ponies;
	int				fd;

	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
		return (1);
	else
	{
		read(fd, &ponies, sizeof(ssize_t));
		if (ponies == 0)
			ponies += 1348;
		close(fd);
		return (ABS(ponies));
	}
}
