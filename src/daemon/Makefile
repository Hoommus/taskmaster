# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/05/20 12:51:40 by vtarasiu          #+#    #+#              #
#    Updated: 2019/06/07 10:49:16 by vtarasiu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = taskmasterd

CC = clang

FLAGS = -DSH=\"$(NAME)\" \
               -Wall  \
               -Wextra \
               -Werror -g #-fsanitize=address

LIB_DIR = ../../lib
INCLUDE_DIR = ../../include

LIB_INCLUDE = -I $(LIB_DIR)/libft \
              -I $(LIB_DIR)/ft_printf/include \
              -I $(LIB_DIR)/json-c

INCLUDE = -I $(INCLUDE_DIR) $(LIB_INCLUDE)

HEADER = -I $(INCLUDE_DIR)/ $(LIB_INCLUDE)
SRC_DIR = ./
OBJ_DIR = ./obj/

LIBFT_DIR = $(LIB_DIR)/libft
PRINTF_DIR = $(LIB_DIR)/ft_printf
PRINTF_LIB_NAME = libftprintf.a

JSON_DIR = $(LIB_DIR)/json-c
JSON_LIB_DIR = $(JSON_DIR)/.libs
JSON_LIB_NAME_STATIC = libjson-c.a
JSON_LIB_NAME_DYNAMIC = libjson-c.dylib

TASKMASTER_COMMON_DIR = ../common
TASKMASTER_COMMON_LIB = $(TASKMASTER_COMMON_DIR)/taskmasterlib.a

DAEMON_SRC = main.c \
			 process_handling.c \
			 ft_vector.c

CONFIG_DIR = config/
CONFIG_SRC = config_load.c

NETWORK_DIR = network/
NETWORK_SRC = sockets_manipulations.c client_interactions.c thread_pool.c \
              serve_invalid.c \
              serve_status.c \
              serve_heartbeat.c \
              serve_stop.c

OBJ = $(addprefix $(OBJ_DIR), $(DAEMON_SRC:.c=.o))                        \
      $(addprefix $(OBJ_DIR)$(CONFIG_DIR), $(CONFIG_SRC:.c=.o))           \
      $(addprefix $(OBJ_DIR)$(NETWORK_DIR), $(NETWORK_SRC:.c=.o))

all: $(NAME)

standalone: libs $(NAME)

$(NAME): prepare $(OBJ) libs
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) $(HEADER) \
	$(PRINTF_DIR)/$(PRINTF_LIB_NAME) $(JSON_LIB_DIR)/$(JSON_LIB_NAME_STATIC) \
	$(TASKMASTER_COMMON_LIB)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	$(CC) $(FLAGS) $(HEADER) -o $@ -c $< ;

prepare:
	@mkdir -p $(OBJ_DIR)$(CONFIG_DIR)
	@mkdir -p $(OBJ_DIR)$(NETWORK_DIR)

libs:
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

clean:
	/bin/rm -rf $(OBJ_DIR)

fclean: clean
	/bin/rm -f $(NAME)

re: fclean all
