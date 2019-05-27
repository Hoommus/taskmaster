/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_basic.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/31 14:46:06 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/27 15:15:04 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include "line_editing.h"
#include "taskmaster_cli.h"

static void				tstp(int sig)
{
	sig = 0;
}

// TODO: abort any connection on sigint
static void				handle_sigint(int sig)
{
	g_interrupt = sig;
	g_shell->last_status = 1;
}

static void				resize(int sig)
{
	struct winsize	size;

	if (sig == SIGWINCH)
	{
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
		g_shell->ws_row = size.ws_row;
		g_shell->ws_col = size.ws_col;
		carpos_update(POS_CURRENT);
	}
}

#ifdef __linux__

void					setup_signal_handlers(void)
{
	struct sigaction	action;

	ft_bzero(&action, sizeof(struct sigaction));
	action.sa_handler = &tstp;
	sigaction(SIGTSTP, &action, NULL);
	action.sa_handler = &handle_sigint;
	sigaction(SIGINT, &action, NULL);
	signal(SIGWINCH, &resize);
	signal(SIGPIPE, &tstp);
	signal(SIGCHLD, SIG_DFL);
}

#elif defined(__APPLE__)

void					setup_signal_handlers(void)
{
	struct sigaction	action;

	ft_bzero(&action, sizeof(struct sigaction));
	action.__sigaction_u.__sa_handler = &tstp;
	sigaction(SIGTSTP, &action, NULL);
	action.__sigaction_u.__sa_handler = &handle_sigint;
	sigaction(SIGINT, &action, NULL);
	signal(SIGWINCH, &resize);
	signal(SIGPIPE, &tstp);
	signal(SIGCHLD, SIG_DFL);
}

#endif
