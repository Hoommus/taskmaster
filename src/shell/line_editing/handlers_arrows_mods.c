/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handlers_arrows_mods.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/18 15:19:03 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/02 16:32:37 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "line_editing.h"
#include "taskmaster_cli.h"

static bool		is_buffer_symbol_at_index_wsp(int64_t index)
{
	return (buff_char_at_equals(index, " ") ||
			buff_char_at_equals(index, "\t") ||
			buff_char_at_equals(index, "\n") ||
			buff_char_at_equals(index, "\r") ||
			buff_char_at_equals(index, "\a"));
}

void			handle_alt_left(union u_char key)
{
	int64_t			i;

	if (key.lng != K_ALT_LEFT)
		return ;
	i = g_shell->buffer->iterator;
	if (!is_buffer_symbol_at_index_wsp(g_shell->buffer->iterator))
		i--;
	while (i >= 0 && (is_buffer_symbol_at_index_wsp(i)))
		i--;
	while (i >= 0)
	{
		if (is_buffer_symbol_at_index_wsp(i))
			break ;
		i--;
	}
	caret_move(ABS(g_shell->buffer->iterator - i) - 1, D_LEFT);
	g_shell->buffer->iterator = (u_int64_t)(i + 1);
}

void			handle_alt_right(union u_char key)
{
	int64_t			i;

	if (key.lng != K_ALT_RIGHT)
		return ;
	i = g_shell->buffer->iterator;
	if (is_buffer_symbol_at_index_wsp(g_shell->buffer->iterator))
		while (i < (int64_t)g_shell->buffer->size
			&& is_buffer_symbol_at_index_wsp(i))
			i++;
	while (i < (int64_t)g_shell->buffer->size)
	{
		if (is_buffer_symbol_at_index_wsp(i))
			break ;
		i++;
	}
	caret_move(ABS(i - g_shell->buffer->iterator), D_RIGHT);
	g_shell->buffer->iterator = i;
}

void			handle_ctrl_w(union u_char key)
{
	int64_t				i;

	if (key.lng != K_CTRL_W)
		return ;
	i = g_shell->buffer->iterator - 1;
	while (i >= 0 && is_buffer_symbol_at_index_wsp(i))
		i--;
	while (i >= 0)
	{
		if (is_buffer_symbol_at_index_wsp(i))
			break ;
		i--;
	}
	i = i == -1 ? 0 : i;
	caret_move(ABS(g_shell->buffer->iterator - i + 1), D_LEFT);
	pb_copy(g_shell->buffer->iterator - i + 1, g_shell->buffer->iterator);
	buff_clear_part(g_shell->buffer->iterator - i, g_shell->buffer->iterator);
	g_shell->buffer->iterator = i;
	buffer_redraw();
}
