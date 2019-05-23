/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_command.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/05 17:50:36 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/11 13:23:48 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell_script.h"
#include "shell_builtins.h"

static int				run_builtin(const char **args)
{
	extern struct s_builtin	g_builtins[];
	int						i;

	i = 0;
	while (args && args[0] && g_builtins[i].name)
	{
		if (ft_strcmp(args[0], g_builtins[i].name) == 0)
			return (g_builtins[i].function((const char **)args + 1));
		i++;
	}
	return (-512);
}

int			exec_command(const t_node *command_node)
{
	char		**args;
	int			status;
	int			i;
	char		*swap;

	args = command_node->command_args;
	i = -1;
	while (args && args[++i])
	{
		swap = expand(args[i]);
		ft_memdel((void **)&(args[i]));
		args[i] = swap;
	}
	status = 0;
	if (!g_interrupt && args != NULL && args[0] != NULL)
		status = run_builtin((const char **)args);
	if (status == -512)
		dprintf(2, ERR_COMMAND_NOT_FOUND, args[0]);
	return (status);
}
