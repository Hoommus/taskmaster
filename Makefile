# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/05/18 15:04:56 by vtarasiu          #+#    #+#              #
#    Updated: 2019/05/27 15:18:22 by vtarasiu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


SHELL_MODULE = src/shell
DAEMON_MODULE = src/daemon

SHELL_BINARY = taskmaster-cli
DAEMON_BINARY = taskmasterd

BINARIES = $(SHELL_MODULE)/$(SHELL_BINARY) $(DAEMON_MODULE)/$(DAEMON_BINARY)

all: prepare
	make -C $(SHELL_MODULE)
	make -C $(DAEMON_MODULE)
	mv $(SHELL_MODULE)/$(SHELL_BINARY) ./$(SHELL_BINARY)
	mv $(DAEMON_MODULE)/$(DAEMON_BINARY) ./$(DAEMON_BINARY)

prepare:
	@cd lib/json-c ; \
	if ! [ -f ./configure ] ; then \
	    sh autogen.sh ; \
	fi ; \
	./configure >/dev/null ;\

clean:
	make -C $(SHELL_MODULE) clean
	make -C $(DAEMON_MODULE) clean

fclean:
	make -C $(SHELL_MODULE) fclean
	make -C $(DAEMON_MODULE) fclean
	/bin/rm -f $(SHELL_BINARY)
	/bin/rm -f $(DAEMON_BINARY)

re: fclean all

love:
	@echo "Not all."

.PHONY: all clean fclean re
