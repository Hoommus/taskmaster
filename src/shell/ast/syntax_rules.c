/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_rules.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/03 16:28:37 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/05/15 19:32:02 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell_script_parser.h"

const t_rule g_complete_command = {
	.token = TOKEN_NOT_APPLICABLE,
	.expands_to = {
		{&g_linebreak, &g_list, &g_separator, &g_linebreak},
		{&g_linebreak, &g_list, &g_linebreak},
		{&g_linebreak}
	},
	.human_readable = "complete_command",
	.tree_builder = NULL
};

const t_rule g_list = {
	.token = TOKEN_NOT_APPLICABLE,
	.expands_to = {
		{&g_simple_command, &g_list_dash}
	},
	.human_readable = "list_alt",
	.tree_builder = NULL
};

const t_rule g_list_dash = {
	.token = TOKEN_NOT_APPLICABLE,
	.expands_to = {
		{&g_separator, &g_simple_command, &g_list_dash},
		{&g_empty_token}
	},
	.human_readable = "list_dash",
	.tree_builder = &list_build
};

const t_rule g_simple_command = {
	.token = TOKEN_NOT_APPLICABLE,
	.expands_to = {
		{&g_cmd_word, &g_cmd_suffix},
		{&g_cmd_word}
	},
	.human_readable = "simple_command",
	.tree_builder = &simple_command_build
};

const t_rule g_cmd_word = { TOKEN_WORD, {{0}}, "cmd_word", NULL };

const t_rule g_cmd_suffix = {
	.token = TOKEN_NOT_APPLICABLE,
	.expands_to = {
		{&g_word_token, &g_cmd_suffix_dash},
	},
	.human_readable = "cmd_suffix_alt",
	.tree_builder = NULL
};
const t_rule g_cmd_suffix_dash = {
	.token = TOKEN_NOT_APPLICABLE,
	.expands_to = {
		{&g_word_token, &g_cmd_suffix_dash},
		{&g_empty_token}
	},
	.human_readable = "cmd_suffix_dash",
	.tree_builder = NULL
};

const t_rule g_newline_list = {
	.token = TOKEN_NOT_APPLICABLE,
	.expands_to = {
		{&g_newline_token, &g_newline_list_dash}
	},
	.human_readable = "newline_list_alt",
	.tree_builder = NULL
};

const t_rule g_newline_list_dash = {
	.token = TOKEN_NOT_APPLICABLE,
	.expands_to = {
		{&g_newline_token, &g_newline_list_dash},
		{&g_empty_token}
	},
	.human_readable = "newline_list_dash",
	.tree_builder = NULL
};

const t_rule g_linebreak = {
	.token = TOKEN_NOT_APPLICABLE,
	{
		{&g_newline_list},
		{&g_empty_token}
	},
	.human_readable = "linebreak",
	.tree_builder = NULL
};

const t_rule g_separator_op = {
	.token = TOKEN_NOT_APPLICABLE,
	.expands_to = {
		{&g_semicolon_token},
		{&g_ampersand_token}
	},
	.human_readable = "separator_op",
	.tree_builder = NULL
};

const t_rule g_semicolon_list_dash = {
	.token = TOKEN_NOT_APPLICABLE,
	.expands_to = {
		{&g_semicolon_token}, {&g_semicolon_list_dash},
		{&g_empty_token}
	},
	.human_readable = "semicolon_list_dash",
	.tree_builder = NULL
};

const t_rule g_separator = {
	.token = TOKEN_NOT_APPLICABLE,
	.expands_to = {
		{&g_separator_op, &g_linebreak},
		{&g_newline_list}
	},
	.human_readable = "separator",
	.tree_builder = NULL
};

const t_rule g_semicolon_token = {
	TOKEN_SEMICOLON,
	{{NULL}},
	"semicolon_t",
	.tree_builder = NULL
};

const t_rule g_newline_token = {
	TOKEN_NEWLINE,
	{{0}},
	"newline_t",
	.tree_builder = NULL
};

const t_rule g_empty_token = {
	TOKEN_EMPTY,
	{{0}},
	"empty_t",
	.tree_builder = NULL
};

const t_rule g_word_token = {
	TOKEN_WORD,
	{{0}},
	"word_t",
	.tree_builder = NULL
};
