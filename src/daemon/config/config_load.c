/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_load.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/21 18:24:56 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/23 13:20:09 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_daemon.h"
#include "json.h"

void		load_config(const char *filename, __unused bool is_update)
{
	json_object		*root;

	root = json_object_from_file(filename);
}
