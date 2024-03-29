/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   context_switch.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/10 12:09:35 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/04 16:02:45 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "taskmaster_cli.h"

void			context_deep_free(t_context **context)
{
	struct s_fd_lst		*list;
	struct s_fd_lst		*swap;

	environ_deallocate_vector((*context)->environ);
	list = (*context)->fd_list;
	while (list)
	{
		swap = list->next;
		if (list->current > 2)
			close(list->current);
		free(list->label);
		free(list);
		list = swap;
	}
	ft_memdel((void **)&((*context)->term_config));
	ft_memdel((void **)context);
}

/*
** If to_which is NULL, restores context from backup with pretty much the same
** algorithm
**
** Remember that dup2 does nothing, if arguments are equal, so there will not be
** any excessive duplications, if context was shallow duplicated
*/

void			context_switch(t_context *to_which)
{
	struct s_fd_lst		*list;

	if (!to_which)
		to_which = g_shell->context_backup;
	else
		g_shell->context_backup = g_shell->context_current;
	g_shell->context_current = to_which;
	list = to_which->fd_list;
	while (list)
	{
		if (list->current == -1)
			close(list->original);
		else
			dup2(list->current, list->original);
		list = list->next;
	}
	TERM_APPLY_CONFIG(to_which->term_config);
}

static void		duplicate_environ(t_context *new_context,
	const t_context *context)
{
	t_var		var;
	u_int32_t	i;

	new_context->environ = environ_create_vector(context->environ->capacity);
	i = new_context->environ->size;
	new_context->environ->size = context->environ->size;
	while (i < new_context->environ->size)
	{
		ft_bzero(&var, sizeof(t_var));
		var.key = ft_strdup(((t_var *)context->environ->array + i)->key);
		var.value = ft_strdup(((t_var *)context->environ->array + i)->value);
		var.scope = ((t_var *)context->environ->array + i)->scope;
		var.hash = ((t_var *)context->environ->array + i)->hash;
		var.key_hash = ((t_var *)context->environ->array + i)->key_hash;
		((t_var *)new_context->environ->array)[i] = var;
		i++;
	}
}

/*
** Creates a full copy of provided context
** If with_dup is set to 'true', duplicates all fds with corresponding syscall
** and duplicates environ.
** Otherwise, new context will share same file descriptors as "parent" context
** and the same pointer to environ (to modify shell environment without hassle).
*/

t_context		*context_duplicate(const t_context *context, bool with_dup)
{
	t_context			*new;

	new = ft_memalloc(sizeof(t_context));
	new->term_config = ft_memalloc(sizeof(struct termios));
	if (context->term_config != NULL)
		ft_memcpy(new->term_config, context->term_config,
			sizeof(struct termios));
	if (with_dup)
		duplicate_environ(new, context);
	else
		new->environ = context->environ;
	return (new);
}
