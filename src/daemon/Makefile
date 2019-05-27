# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/05/20 12:51:40 by vtarasiu          #+#    #+#              #
#    Updated: 2019/05/27 19:48:59 by obamzuro         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = taskmasterd

CC = clang

FLAGS = -DSH=\"$(NAME)\" \
               -Wall  \
               -Wextra \
               -Werror -g

LIB_DIR = ../../lib
INCLUDE_DIR = ../../include

LIB_INCLUDE = -I $(LIB_DIR)/libft \
              -I $(LIB_DIR)/ft_printf/include \
              -I $(LIB_DIR)/json-c

INCLUDE = -I $(INCLUDE_DIR) $(LIB_INCLUDE)

HEADER = -I $(INCLUDE_DIR)/ $(LIB_INCLUDE)
SRC_DIR = ./
OBJ_DIR = ./obj/

PRINTF_DIR = $(LIB_DIR)/ft_printf
PRINTF_LIB_NAME = libftprintf.a

JSON_DIR = $(LIB_DIR)/json-c
JSON_LIB_NAME_STATIC = libjson-c.a
JSON_LIB_NAME_DYNAMIC = libjson-c.dylib

DAEMON_SRC = main.c\
			 process_handling.c

CONFIG_DIR = config/
CONFIG_SRC = config_load.c

NETWORK_DIR = network/
NETWORK_SRC = sockets_manipulations.c

OBJ = $(addprefix $(OBJ_DIR), $(DAEMON_SRC:.c=.o))                        \
      $(addprefix $(OBJ_DIR)$(CONFIG_DIR), $(CONFIG_SRC:.c=.o))           \
      $(addprefix $(OBJ_DIR)$(NETWORK_DIR), $(NETWORK_SRC:.c=.o))

all: $(NAME)

$(NAME): prepare $(OBJ) libs
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) $(HEADER) $(JSON_LIB_NAME_STATIC) $(PRINTF_DIR)/$(PRINTF_LIB_NAME)

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
	    ./configure    ; \
	    make ; \
	fi ;
	cp $(JSON_DIR)/.libs/$(JSON_LIB_NAME_STATIC) ./$(JSON_LIB_NAME_STATIC)

clean:
	/bin/rm -rf $(OBJ_DIR)
	/bin/rm -f $(NAME)

fclean:
	/bin/rm -f $(NAME)

re: fclean all
