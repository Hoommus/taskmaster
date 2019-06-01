# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/05/18 15:04:56 by vtarasiu          #+#    #+#              #
#    Updated: 2019/05/31 18:31:57 by vtarasiu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SHELL_MODULE = src/shell
DAEMON_MODULE = src/daemon
COMMON_MODULE = src/common

SHELL_BINARY = taskmaster-cli
DAEMON_BINARY = taskmasterd

COMMON_NAME = taskmasterlib.a

LIB_DIR = lib

PRINTF_DIR = $(LIB_DIR)/ft_printf
LIBFT_DIR = $(LIB_DIR)/libft

BINARIES = $(SHELL_MODULE)/$(SHELL_BINARY) $(DAEMON_MODULE)/$(DAEMON_BINARY)

JSON_DIR = $(LIB_DIR)/json-c
JSON_LIB_NAME_STATIC = libjson-c.a
JSON_LIB_NAME_DYNAMIC = libjson-c.dylib

all: libs
	make -C $(COMMON_MODULE)
	make -C $(SHELL_MODULE)
	make -C $(DAEMON_MODULE)
	mv $(SHELL_MODULE)/$(SHELL_BINARY) ./$(SHELL_BINARY)
	mv $(DAEMON_MODULE)/$(DAEMON_BINARY) ./$(DAEMON_BINARY)

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

clean:
	make -C $(COMMON_MODULE) clean
	make -C $(SHELL_MODULE) clean
	make -C $(DAEMON_MODULE) clean

fclean:
	make -C $(COMMON_MODULE) fclean
	make -C $(SHELL_MODULE) fclean
	make -C $(DAEMON_MODULE) fclean
	/bin/rm -f $(SHELL_BINARY)
	/bin/rm -f $(DAEMON_BINARY)

re: fclean all

love:
	@echo "Not all."

.PHONY: all clean fclean re
