# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/05/20 12:51:40 by vtarasiu          #+#    #+#              #
#    Updated: 2019/05/22 15:04:25 by vtarasiu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = taskmasterd

CC = clang

FLAGS = -DSH=\"$(NAME)\" \
               -Wall  \
               -Wextra \
               -Werror -g -fsanitize=address

LIB_DIR = ../../lib

LIB_INCLUDE = -I $(LIB_DIR)/libft -I $(LIB_DIR)/ft_printf/include
INCLUDE = -I ../../include $(LIB_INCLUDE)

HEADER = -I ../../include/ $(LIB_INCLUDE)
SRC_DIR = ./
OBJ_DIR = ./obj/

PRINTF_DIR = $(LIB_DIR)/ft_printf
PRINTF_LIB_NAME = libftprintf.a

SRC = main.c

CONFIG_DIR = config/
CONFIG_SRC = config_load.c

NETWORK_DIR = network/
NETWORK_SRC = sockets_manipulations.c

all:
	$(CC) $(FLAGS) -o $(NAME) $(SRC) $(NETWORK_DIR)/$(NETWORK_SRC) $(INCLUDE)

clean:
	/bin/rm -f $(NAME)
