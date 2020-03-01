# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: akorzhak <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/01/13 18:35:56 by akorzhak          #+#    #+#              #
#    Updated: 2018/01/13 18:35:58 by akorzhak         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so

C = gcc

CFLAG = -Wall -Wextra -Werror

LIBDIR = libft

LIB = $(LIBDIR)/libft.a

SRCDIR = src

SRC = malloc.c

IDIR = include

INC = $(IDIR)/libftprintf.h

OBJ = $(SRC:.c=.o)

.PHONY: all clean fclean

%.o: $(SRCDIR)/%.c
	@$(C) -c $<

all: $(NAME)

$(NAME): $(OBJ)
	@make -C $(LIBDIR)
	@cp $(LIB) $(NAME)
#	@ar rc $(NAME) $(OBJ)
    @ln -s $(NAME) libft_malloc.so
#	@ranlib $(NAME)
	@echo libft_malloc.so has been successfully created :')'

clean:
	@make clean -C $(LIBDIR)
	@/bin/rm -f $(OBJ) *~
	@echo object files have been removed

fclean: clean
	@make fclean -C $(LIBDIR)
	@/bin/rm -f $(NAME)
	@echo libftprintf.a has been removed :'('

re: fclean all
