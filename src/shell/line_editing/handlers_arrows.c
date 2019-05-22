/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handlers_arrows.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/16 12:28:37 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/10 17:45:41 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "line_editing.h"
#include "shell_history.h"
#include "taskmaster_cli.h"

extern t_history		*g_history;

void	handle_up(union u_char key)
{
	struct s_history_entry	*entry;
	char					*tmp;

	if ((key.lng == K_UP || key.lng == K_UP_FALLBACK)
		&& (!g_shell->buffer->iterator || !g_shell->buffer->size
			|| g_shell->buffer->iterator == g_shell->buffer->size)
		&& g_history->iterator > 0)
	{
		tmp = buff_get_part(0, UINT64_MAX);
		while (tmp && (entry = history_get_entry(--g_history->iterator)))
			if (ft_strcmp(entry->command, tmp))
				break ;
		ft_strdel(&tmp);
		if (!entry)
			return ;
		caret_move(g_shell->buffer->iterator, D_LEFT);
		carpos_update(POS_PROMPT);
		buff_clear(0);
		tputs(tgetstr("cd", NULL), 1, &ft_putc);
		buff_insert_string_at(0, entry->command);
		ft_printf("%s", entry->command);
		if (carpos_update(POS_CUSTOM1)->row == g_shell->ws_row)
			carpos_adjust_db(linecount(entry->command, g_shell->ws_col) - 1);
	}
}

void	handle_down(union u_char key)
{
	struct s_history_entry	*entry;
	char					*tmp;

	if ((key.lng == K_DOWN || key.lng == K_DOWN_FALLBACK)
		&& history_get_entry(g_history->iterator) != NULL)
	{
		tmp = buff_get_part(0, UINT64_MAX);
		while ((entry = history_get_entry(++g_history->iterator)))
			if (ft_strcmp(entry->command, tmp))
				break ;
		ft_strdel(&tmp);
		caret_move(g_shell->buffer->iterator, D_LEFT);
		buff_clear(0);
		tputs(tgetstr("cd", NULL), 1, &ft_putc);
		if (!entry)
			return ;
		buff_insert_string_at(0, entry->command);
		ft_printf("%s", entry->command);
	}
}

void	handle_left(union u_char key)
{
	if ((key.lng == K_LEFT || key.lng == K_LEFT_FALLBACK)
		&& g_shell->buffer->iterator > 0)
	{
		if ((g_shell->input_state == STATE_HEREDOC ||
			g_shell->input_state == STATE_HEREDOCD ||
			g_shell->input_state == STATE_QUOTE ||
			g_shell->input_state == STATE_DQUOTE ||
			g_shell->input_state == STATE_BQUOTE ||
			g_shell->input_state == STATE_ESCAPED)
			&& buff_char_at_equals(g_shell->buffer->iterator - 1, "\n"))
			return ;
		caret_move(1, D_LEFT);
		g_shell->buffer->iterator--;
	}
}

void	handle_right(union u_char key)
{
	if ((key.lng == K_RIGHT || key.lng == K_RIGHT_FALLBACK)
		&& g_shell->buffer->iterator < g_shell->buffer->size
		&& ft_strcmp(buff_char_at(g_shell->buffer->iterator), "\0") != 0)
	{
		caret_move(1, D_RIGHT);
		g_shell->buffer->iterator++;
	}
}
