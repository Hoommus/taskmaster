/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/31 14:45:42 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/06/07 16:18:42 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"
#include "shell_builtins.h"

struct s_resolver	g_resolvers[] = {
	{REQUEST_STATUS, &parse_response_status},
	{0, NULL}
};

struct s_builtin	g_builtins[] = {
	{ "connect",    &tm_connect,    NULL,                      BUILTIN_SHELL  },
	{ "disconnect", &tm_disconnect, NULL,                      BUILTIN_REMOTE },
	{ "drop",       &tm_disconnect, NULL,                      BUILTIN_REMOTE },
	{ "exit",       &hs_exit,       NULL,                      BUILTIN_SHELL  },
	{ "echo",       &hs_echo,       NULL,                      BUILTIN_REMOTE },
	{ "history",    &hs_history,    NULL,                      BUILTIN_SHELL  },
	{ "help",       &hs_help,       NULL,                      BUILTIN_SHELL  },
	{ "quit",       &hs_exit,       NULL,                      BUILTIN_SHELL  },
	{ "status",     &tm_status,     &tm_status_help,           BUILTIN_REMOTE },
	{ "stop",       &tm_stop,       NULL,                      BUILTIN_REMOTE },
	{ "tokenizer",  &hs_tokenizer,  NULL,                      BUILTIN_SHELL  },
	{ NULL,         NULL,           NULL,                      BUILTIN_SHELL  }
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

	i = 0;
	while (args != NULL && args[0] != NULL && g_builtins[i].name)
		if (strcmp(g_builtins[i].name, args[0]) == 0 && g_builtins[i].help)
			return (g_builtins[i].help());
	ft_printf(SH " local commands:\n");
	i = 0;
	while (g_builtins[i].name != NULL)
		if (g_builtins[i].clazz == BUILTIN_SHELL)
			ft_printf("%s\n", g_builtins[i++].name);
	ft_printf("\n" SH " commands for remote daemon:\n");
	i = 0;
	while (g_builtins[i].name != NULL)
		if (g_builtins[i].clazz == BUILTIN_REMOTE)
			ft_printf("%s\n", g_builtins[i++].name);
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
