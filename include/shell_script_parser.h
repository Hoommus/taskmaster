/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_script_parser.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/10 13:17:59 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/04/23 16:16:56 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHELL_SCRIPT_PARSER_H
# define SHELL_SCRIPT_PARSER_H

# define INDENT_0 "│ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │"
# define INDENT INDENT_0 INDENT_0 INDENT_0 INDENT_0 INDENT_0 INDENT_0 INDENT_0
# define IS_TERMINAL(rule) (!(rule)->expands_to[0] || !(rule)->expands_to[0][0])

# define LOG_INSIDE "%.*s%s inside [ \x1b[36m%s\x1b[0m ];\n"
# define LOG_OUTSIDE "%.*s%s outside [ \x1b[36m%s\x1b[0m ];\n"
# define LOG_BUILDER "%.*s%s building [ \x1b[36m%s\x1b[0m ];\n"
# define LOG_TERMINAL "%.*s  %s terminal in rule [%s] at %s (%s) \n"
# define LOG_EMPTY "%.*s    \x1b[35m%s empty allowed, validating\x1b[0m\n"
# define LOG_EXPECT "%.*s    \x1b[32m%s expected: (%s); got (%s)\x1b[0m\n"

# include "shell_script.h"

typedef struct					s_parser_state
{
	const struct s_syntax_rule	*rule;
	t_token						*list_head;
	t_token						*list_offset;
	int							depth;
	bool						logging;
}								t_state;

/*
** Syntax rule is an entity that contains an array of possible expansions
** which define language grammar.
**
** Rules which given rule expands to are stored in 'expands_to' field.
** The expands_to field is a two dimensional NULL-terminated array of pointers
** to s_syntax_rule variables declared in syntax_rules.h.
** Basically, it can hold up to 9 cases of given rule each of which
** can hold up to 9 rules and tokens to which this rule expands.
** For example, this syntax rule from BNF will be translated to this
** structure as follows:
**
** Original:
**  command : simple_command
**          | compound_command
**          | compound_command redirect_list
**          | function_definition
**          ;
**
** This shell struct:
**  t_rule command = {
**      {0}, // because it cannot be any token
**
**      // Token stream must correspond to either of these .expands_to rules
**      // to be valid 'command'
**      .expands_to = {
**          { &simple_command },      // command can be solely simple_command
**          { &compound_command },    // OR compound_command
**          // OR compound_command which is followed by redirect_list
**          { &compound_command, &redirect_list },
**          { &function_definition }, // OR be a function definition
**      },
**      .human_readable = "command",
**      .tree_builder = &command_build
**  };
*/

/*
** Forward declarations
*/
typedef struct s_build_result	t_bresult;
struct s_result;

typedef t_bresult				*(*t_builder) (const t_state *state,
	struct s_result *last_build);

typedef struct					s_syntax_rule
{
	const enum e_token_type					token;
	const struct s_syntax_rule *restrict	expands_to[8][6];
	const char *restrict const				human_readable;
	t_builder								tree_builder;
} __attribute__((packed))								t_rule;

typedef struct					s_syntax_error
{
	short			code;
	char			*text;
}								t_error;

struct							s_build_result
{
	t_node			*root;
	const t_rule	*request;
};

struct							s_result
{
	t_error			*error;
	t_bresult		*ast;
	t_bresult		*backup_ast;
	int				consumed;
	bool			fatal;
	bool			valid;
};

t_token							*offset_list(t_token *list, int offset);
struct s_result					is_syntax_valid(t_state const prev);

/*
** Rule builders
*/
t_bresult						*simple_command_build(const t_state *state,
												struct s_result *last_build);
t_bresult						*list_build(const t_state *state,
												struct s_result *last_build);

t_node							*ast_new_node(void *value,
												enum e_node_type node_type);
void							ast_free_recursive(t_node *node);

t_bresult						*insert_left_recursive(t_bresult *bresult,
									t_node *parent, t_node *insertion);

extern const t_rule				g_complete_command;
extern const t_rule				g_list;
extern const t_rule				g_list_dash;

extern const t_rule				g_simple_command;
extern const t_rule				g_cmd_word;
extern const t_rule				g_cmd_suffix;
extern const t_rule				g_cmd_suffix_dash;
extern const t_rule				g_newline_list;
extern const t_rule				g_newline_list_dash;
extern const t_rule				g_linebreak;
extern const t_rule				g_separator_op;
extern const t_rule				g_semicolon_list_dash;
extern const t_rule				g_separator;

extern const t_rule				g_semicolon_token;
extern const t_rule				g_newline_token;
extern const t_rule				g_empty_token;
extern const t_rule				g_word_token;

#endif
