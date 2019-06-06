/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_builtins.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/15 15:56:22 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/04 15:56:33 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHELL_BUILTINS_H
# define SHELL_BUILTINS_H

# include "libft.h"
# include "get_next_line.h"
# include "ft_printf.h"
# include "shell_environ.h"

struct					s_builtin
{
	char	*name;
	int		(*function) (const char **);
};

void					print_var(const t_var *var);

/*
** Builtins (builtins/ *.c)
*/
int						tm_disconnect(const char **args);
int						tm_connect(const char **args);
int						hs_echo(const char **args);
int						hs_help(const char **args);
int						hs_exit(const char **args);
int						hs_where(const char **args);
int						hs_history(const char **args);
int						hs_tokenizer(const char **args);

int						tm_status(const char **args);
int						tm_stop(const char **args);

#endif
