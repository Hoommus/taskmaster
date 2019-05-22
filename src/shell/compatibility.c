/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   compatibility.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/27 13:10:09 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/02/27 13:10:09 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"

#ifdef __linux__

int				gethostname(char *arr, size_t size)
{
	ft_memcpy(arr, "linux\0", size);
	return (0);
}

#endif
