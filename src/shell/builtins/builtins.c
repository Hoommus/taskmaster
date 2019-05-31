/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/31 14:45:42 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/23 15:37:04 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"
#include "shell_builtins.h"

struct s_resolver	g_resolvers[] = {
	{REQUEST_STATUS, &parse_response_status},
	{0, NULL}
};

struct s_builtin	g_builtins[] = {
	{"connect", &tm_connect},
	{"disconnect", &tm_disconnect},
	{"drop", &tm_disconnect},
	{"echo", &hs_echo},
	{"exit", &hs_exit},
	{"history", &hs_history},
	{"help", &hs_help},
	{"quit", &hs_exit},
	{"status", &tm_status},
	{"tokenizer", &hs_tokenizer},
	{NULL, NULL}
};

int					hs_echo(const char **args)
{
	char	*str;

	if (g_shell->daemon == NULL)
	{
		ft_dprintf(2, "You are not connected to any Taskmaster\n");
		return (2);
	}
	if (args == NULL || args[0] == NULL)
	{
		ft_dprintf(g_shell->daemon->socket_fd, "\n");
		ft_dprintf(1, "\n");
		return (0);
	}
	str = ft_strarr_join(" ", (char **)args);
	ft_dprintf(g_shell->daemon->socket_fd, "%s\n", str);
	ft_dprintf(1, "%s\n", str);
	free(str);
	return (0);
}

int					hs_help(const char **args)
{
	int		i;

	ft_printf(SH " commands:\n");
	i = 0;
	while (g_builtins[i].name != NULL)
		ft_printf("%s\n", g_builtins[i++].name);
	*args = args[0];
	return (0);
}

int					hs_exit(const char **args)
{
	TERM_APPLY_CONFIG(g_shell->context_original->term_config);
	if (args && *args)
	{
		if (!is_string_numeric(*args, 10))
		{
			ft_dprintf(2, SH ": exit: %s: numeric argument required\n", *args);
			exit(2);
		}
		else
			exit(ft_atoi(*args));
	}
	exit(0);
}
