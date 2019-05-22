/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handlers_editing.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/18 13:45:39 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/06 15:53:17 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "line_editing.h"
#include "taskmaster_cli.h"

void	handle_backspace(union u_char key)
{
	if (key.lng == K_BSP && g_shell->buffer->iterator > 0)
	{
		if ((g_shell->input_state > STATE_NON_INTERACTIVE) &&
			buff_char_at_equals(g_shell->buffer->iterator - 1, "\n"))
			return ;
		handle_left((union u_char){K_LEFT});
		buff_del_symbol(g_shell->buffer->iterator);
		tputs(tgetstr("dc", NULL), 1, &ft_putc);
		buffer_redraw();
	}
}

void	handle_del(union u_char key)
{
	if (key.lng == K_DEL)
	{
		if (g_shell->buffer->iterator <= g_shell->buffer->size)
			tputs(tgetstr("dc", NULL), 1, &ft_putc);
		if (g_shell->buffer->iterator < g_shell->buffer->size)
		{
			buff_del_symbol(g_shell->buffer->iterator);
			buffer_redraw();
		}
	}
}

void	handle_home(union u_char key)
{
	if (key.lng == K_HOME)
	{
		caret_move(g_shell->buffer->iterator, D_LEFT);
		g_shell->buffer->iterator = 0;
	}
}

void	handle_end(union u_char key)
{
	if (key.lng == K_END)
	{
		caret_move(g_shell->buffer->size - g_shell->buffer->iterator, D_RIGHT);
		g_shell->buffer->iterator = g_shell->buffer->size;
	}
}
