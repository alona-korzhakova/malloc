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

CFLAG = -Wall -Wextra -Werror -fPIC

INC = -I ./include -I ./ft_printf/includes

PRINTF_DIR = ft_printf/

PRINTF_LIB = $(PRINTF_DIR)libftprintf.a

SRCDIR = src/

SRC = create_new_item.c free.c main.c occupy_free_block.c util.c\
	  find_ptr_in_db.c   init.c  malloc.c  realloc.c

OBJ = $(SRC:.c=.o)

# Colors constants
include Colors.mk

.PHONY: all clean fclean

%.o: $(SRCDIR)%.c
	@$(C) $(CFLAG) -c $< -pthread $(INC)

all: $(NAME)

$(NAME): $(OBJ)
	@make -C $(PRINTF_DIR)
	$(C) $(CFLAGS) -shared -o $(NAME) $(OBJ) $(INC) -Wl,--whole-archive $(PRINTF_LIB) -Wl,--no-whole-archive

	@ln -s $(NAME) libft_malloc.so
	@echo libft_malloc.so has been successfully created :')'

clean:
	@make clean -C $(PRINTF_DIR)
	@/bin/rm -f $(OBJ) *~
	@echo object files have been removed

fclean: clean
	@make fclean -C $(PRINTF_DIR)
	@/bin/rm -f $(NAME)
	@echo libft_malloc.so has been removed :'('

re: fclean all
