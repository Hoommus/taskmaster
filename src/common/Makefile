# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/05/29 12:22:39 by vtarasiu          #+#    #+#              #
#    Updated: 2019/05/29 14:52:19 by vtarasiu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = taskmasterlib.a

CC = clang

FLAGS =        -Wall  \
               -Wextra \
               -Werror -g

LIB_DIR = ../../lib

LIB_INCLUDE = -I $(LIB_DIR)/libft -I $(LIB_DIR)/ft_printf/include -I $(LIB_DIR)/json-c

JSON_DIR = $(LIB_DIR)/json-c
JSON_LIB_DIR = $(JSON_DIR)/.libs
JSON_LIB_NAME_STATIC = libjson-c.a
JSON_LIB_NAME_DYNAMIC = libjson-c.dylib

HEADER = -I ../../include/ $(LIB_INCLUDE)

SRC_DIR = ./
OBJ_DIR = ./obj/

SRC = network_get.c \
      network_queue.c \
      network_send.c

OBJ = $(addprefix $(OBJ_DIR), $(SRC:.c=.o))

LIBS = $(PRINTF_DIR)/$(PRINTF_LIB_NAME) $(JSON_LIB_DIR)/$(JSON_LIB_NAME_STATIC) -ltermcap

all: $(NAME)

$(NAME): prepare $(OBJ)
	@echo "ar rc $(NAME) $(OBJ_DIR)*.o"
	ar rc $(NAME) $(OBJ)
	ranlib $(NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	$(CC) $(FLAGS) $(HEADER) -o $@ -c $< ;

prepare:
	@mkdir -p $(OBJ_DIR)

clean:
	@/bin/rm -rf $(OBJ)
	/bin/rm -rf $(OBJ_DIR)

fclean: clean
	/bin/rm -f $(NAME)

re: fclean all

love:
	@echo "Not standalone."