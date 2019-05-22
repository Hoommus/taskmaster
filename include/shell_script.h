/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_script.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/11/29 14:44:44 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/14 12:49:35 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHELL_SCRIPT_H
# define SHELL_SCRIPT_H

# include "libft.h"
# include "ft_printf.h"
# include <stdbool.h>
# include "taskmaster_cli.h"

# define TOKEN_DELIMITERS " \t\r\a"
# define ISQT(x) (((x) == '\'' || (x) == '"' || (x) == '`') ? (1) : (0))

# define LEXER_OUTSIDE_QUOTE       1
# define LEXER_INSIDE_QUOTE        2
# define LEXER_NEXT_ESCAPED        4
# define LEXER_NEXT_UNQUOTED_DELIM 8
# define LEXER_UNQUOTED_PRINT      16
# define LEXER_BREAK               32

/*
** TODO: Add functions capability:
**         global function database
**         lifespan control
*/

/*
** Do not change order in which these entries appear. They provide easy random
** access to a specific token entry in recognisable by this shell token table.
** @see g_tokens[] src/lexer/tokenizer.c
*/

enum						e_token_type
{
	TOKEN_SEMICOLON,

	TOKEN_WORD,
	TOKEN_NAME,

	TOKEN_NEWLINE,
	TOKEN_EMPTY,

	TOKEN_NOT_APPLICABLE,

	TOKEN_KEYWORD,
	TOKEN_WORD_COMMAND
};

enum						e_node_type
{
	NODE_SEPARATOR,
	NODE_COMMAND,
};

/*
** s_lexer_token is a struct that used on lexical analysis stage. Lexer uses
** information from g_tokens table to parse char input into tokens and literals
** and pass this new stream to syntax analyzer and then parser.
*/

struct						s_lexer_token
{
	char					*text;
	char					*token_name;
	enum e_token_type		type;
	bool					requires_single;
};

typedef struct				s_token
{
	int						line_nbr;
	const char				*value;
	enum e_token_type		type;
	struct s_token			*prev;
	struct s_token			*next;
}							t_token;

typedef struct				s_node
{
	char					**command_args;
	enum e_node_type		node_type;
	struct s_node			*left;
	struct s_node			*right;
}							t_node;

union						u_executor
{
	int		(*exec)(const t_node *node);
	int		(*exec_alt_context)(const t_node *node, struct s_context *context);
};

struct						s_executor
{
	enum e_node_type	node_type;
	union u_executor	executor;
};

extern const struct s_lexer_token	g_tokens[];
extern const struct s_executor		g_executors_table[];

/*
** Lexer
*/

struct s_token				*tokenize(char *str, const char *delimiters);
struct s_token				*new_token(char *value, enum e_token_type type);
void						add_token(t_token **head, t_token **tail,
														t_token *to_add);
t_token						*pop_token(t_token **head, t_token **tail);
void						free_token(struct s_token *token);

enum e_token_type			token_class_contextual(const char *str);
void						free_array(void **array);

/*
** Lexer Preprocess
*/

char						*strip_escaped_nl_and_comments(char *string);

/*
** AST Main
*/

void						run_script(t_token *list_head, bool log_recursion);
int							exec_command(const t_node *command_node);
int							exec_semicolon_iterative(t_node *parent);
int							exec_semicolon_recursive(const t_node *parent);
int							exec_node(const t_node *node);
int							exec_abort(int dummy);

/*
** File reading and executing
*/

int							read_filename(const char *file, char **data);

/*
** Expansions
*/

char						*expand(char *string);

#endif
