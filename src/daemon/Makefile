# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/05/20 12:51:40 by vtarasiu          #+#    #+#              #
#    Updated: 2019/05/23 13:49:49 by vtarasiu         ###   ########.fr        #
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

INCLUDE = -I ../../include $(LIB_INCLUDE)

HEADER = -I ../../include/ $(LIB_INCLUDE)
SRC_DIR = ./
OBJ_DIR = ./obj/

PRINTF_DIR = $(LIB_DIR)/ft_printf
PRINTF_LIB_NAME = libftprintf.a

JSON_DIR = $(LIB_DIR)/json-c
JSON_LIB_NAME_STATIC = libjson-c.a
JSON_LIB_NAME_DYNAMIC = libjson-c.dylib

DAEMON_SRC = main.c

CONFIG_DIR = config/
CONFIG_SRC = config_load.c

NETWORK_DIR = network/
NETWORK_SRC = sockets_manipulations.c

OBJ = $(addprefix $(OBJ_DIR), $(DAEMON_SRC:.c=.o))                        \
      $(addprefix $(OBJ_DIR)$(CONFIG_DIR), $(CONFIG_SRC:.c=.o))           \
      $(addprefix $(OBJ_DIR)$(NETWORK_DIR), $(NETWORK_SRC:.c=.o))

all: $(NAME)

$(NAME): prepare $(OBJ) libs
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) $(PRINTF_DIR)/$(PRINTF_LIB_NAME) $(HEADER) $(JSON_LIB_NAME_STATIC)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	$(CC) $(FLAGS) $(HEADER) -o $@ -c $< ;

prepare:
	@mkdir -p $(OBJ_DIR)$(CONFIG_DIR)
	@mkdir -p $(OBJ_DIR)$(NETWORK_DIR)

libs:
	make -C $(PRINTF_DIR)/
	cd $(JSON_DIR) ; \
	sh autogen.sh  ; \
	./configure    ; \
	make
	mv $(JSON_DIR)/.libs/$(JSON_LIB_NAME_STATIC) ./$(JSON_LIB_NAME_STATIC)

clean:
	/bin/rm -rf $(OBJ_DIR)
	/bin/rm -f $(NAME)

fclean:
	/bin/rm -f $(NAME)

re: fclean all
