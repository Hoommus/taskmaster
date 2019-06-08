# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/03/24 10:11:17 by vtarasiu          #+#    #+#              #
#    Updated: 2019/06/07 17:41:03 by vtarasiu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = taskmaster-cli

CC = clang

FLAGS = -DSH=\"$(NAME)\" \
               -Wall  \
               -Wextra \
               -Werror -g

LIB_DIR = ../../lib

LIB_INCLUDE = -I $(LIB_DIR)/libft -I $(LIB_DIR)/ft_printf/include -I $(LIB_DIR)/json-c

JSON_DIR = $(LIB_DIR)/json-c
JSON_LIB_DIR = $(JSON_DIR)/.libs
JSON_LIB_NAME_STATIC = libjson-c.a
JSON_LIB_NAME_DYNAMIC = libjson-c.dylib

TASKMASTER_COMMON_DIR = ../common
TASKMASTER_COMMON_LIB = $(TASKMASTER_COMMON_DIR)/taskmasterlib.a

HEADER = -I ../../include/ $(LIB_INCLUDE)
SRC_DIR = ./
OBJ_DIR = ./obj/

PRINTF_DIR = $(LIB_DIR)/ft_printf
PRINTF_LIB_NAME = libftprintf.a

SHELL_SRC = args_parsing.c \
            auxilia.c \
            main.c \
            string_hash.c \
            memory.c \
            service_routines.c \
            auxiliary_main.c \
            compatibility.c \
            syscall_wrappers.c \
            init.c \
            shell_environ.c \
            shell_environ_tools.c \
            shell_environ_vector.c \
            shell_environ_memory.c

LEXER_DIR = lexer/
LEXER_SRC = tokenizer.c tokens_mem.c token_word_types.c \
            tokenizer_preprocess.c

AST_DIR = ast/
AST_SRC = ast_exec_main.c \
          exec_command.c \
          execution.c \
          nodes_manipulations.c \
          nodes_memory.c \
          parser.c \
          syntax_rules.c \
          tree_list.c \
          tree_simple_command.c

BUILTIN_DIR = builtins/
BUILTIN_SRC = builtins.c \
              hs_history.c \
              tokenizer_test.c \
              tm_connect.c \
              tm_echo.c \
              tm_status.c \
              tm_stop.c \
              tm_banish.c \
              tm_disconnect.c

INTERFACE_DIR = line_editing/
INTERFACE_SRC = buffer_drawing.c buffer_input.c  \
                cursor_control.c cursor_positions.c \
                buffer_vector.c buffer_vector_tools1.c buffer_vector_tools2.c  \
                buffer_vector_insertions.c buffer_vector_parts.c \
                state_toggles.c \
                handlers_arrows.c handlers_editing.c handlers_engine.c \
                handlers_arrows_mods.c handlers_arrows_vertical.c \
                handlers_clipboard.c \
                pasteboard_interface.c \
                auxiliary_buffer.c auxiliary_le.c

JOB_CONTROL_DIR = job_control/
JOB_CONTROL_SRC = signals_basic.c \
                  context_manipulations.c context_switch.c

EXPANSIONS_DIR = expansions/
EXPANSIONS_SRC = expander_engine.c expand_escaped.c expand_quotes.c

HISTORY_DIR = features/history/
HISTORY_SRC = history.c history_vector.c

OBJ = $(addprefix $(OBJ_DIR), $(SHELL_SRC:.c=.o))                         \
      $(addprefix $(OBJ_DIR)$(AST_DIR), $(AST_SRC:.c=.o))                 \
      $(addprefix $(OBJ_DIR)$(LEXER_DIR), $(LEXER_SRC:.c=.o))             \
      $(addprefix $(OBJ_DIR)$(BUILTIN_DIR), $(BUILTIN_SRC:.c=.o))         \
      $(addprefix $(OBJ_DIR)$(HISTORY_DIR), $(HISTORY_SRC:.c=.o))         \
      $(addprefix $(OBJ_DIR)$(INTERFACE_DIR), $(INTERFACE_SRC:.c=.o))     \
      $(addprefix $(OBJ_DIR)$(EXPANSIONS_DIR), $(EXPANSIONS_SRC:.c=.o))   \
      $(addprefix $(OBJ_DIR)$(JOB_CONTROL_DIR), $(JOB_CONTROL_SRC:.c=.o)) \

all: $(NAME) 

standalone: libs $(NAME)

$(NAME): prepare $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) $(PRINTF_DIR)/$(PRINTF_LIB_NAME) $(HEADER) \
	$(JSON_LIB_DIR)/$(JSON_LIB_NAME_STATIC) $(TASKMASTER_COMMON_LIB) -ltermcap

prepare:
	@mkdir -p $(OBJ_DIR)$(AST_DIR)
	@mkdir -p $(OBJ_DIR)$(LEXER_DIR)
	@mkdir -p $(OBJ_DIR)$(BUILTIN_DIR)
	@mkdir -p $(OBJ_DIR)$(HISTORY_DIR)
	@mkdir -p $(OBJ_DIR)$(INTERFACE_DIR)
	@mkdir -p $(OBJ_DIR)$(EXPANSIONS_DIR)
	@mkdir -p $(OBJ_DIR)$(JOB_CONTROL_DIR)

libs:
	make -C $(LIBFT_DIR)/
	make -C $(PRINTF_DIR)/
	if ! [ -f ./$(JSON_LIB_NAME_STATIC) ] ; then \
	    cd $(JSON_DIR) ; \
	    if ! [ -f ./configure ] ; then \
	        sh autogen.sh  ; \
	    fi ; \
	    if ! [ -f ./.libs/$(JSON_LIB_NAME_STATIC) ] ; then \
	    	./configure    ; \
	    	make ; \
	    fi \
	fi ;

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	$(CC) $(FLAGS) $(HEADER) -o $@ -c $< ;

install:
	if [ grep ~/.brew/bin $PATH 2>/dev/null ] ; \
	then \
	    mkdir -p ~/.brew/bin/   ; \
	    cp $(NAME) ~/.brew/bin/ ; \
	    echo "\n export PATH=\$$PATH:\$$HOME/.brew/bin" >> ~/.zshrc ; \
	    source ~/.zshrc         ; \
	    echo "$(NAME) installed"   ; \
	else \
	    cp $(NAME) ~/.brew/bin/ ; \
	    echo "$(NAME) updated"     ; \
	fi ;

clean:
	@echo "rm -rf $(OBJ_DIR)*.o"
	@/bin/rm -rf $(OBJ)
	/bin/rm -rf $(OBJ_DIR)


fclean: clean
	/bin/rm -f $(NAME)
	/bin/rm -f $(LIB_NAME)

re: fclean
	make all

love:
	@echo "Not all."

.PHONY: clean all fclean re love
