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

INC = -I ./include -I ./libft

LIB_DIR = libft/

LIB = $(LIB_DIR)libft.a

SRCDIR = src/

PATH_OBJ = obj/

SRC = create_new_item.c free.c occupy_free_block.c util.c\
	  find_ptr_in_db.c  init.c malloc.c realloc.c calloc.c

OBJ = $(patsubst %.c, $(PATH_OBJ)%.o, $(SRC))

# Colors constants
include Colors.mk

.PHONY: all clean fclean

$(PATH_OBJ)%.o: $(addprefix $(SRCDIR),%.c)
	mkdir -p $(PATH_OBJ)
	$(C) $(CFLAG) -c -o $@ $^ -pthread $(INC)

all: $(NAME)

$(NAME): $(OBJ)
	make -C $(LIB_DIR)
	$(C) $(CFLAGS) -shared -o $(NAME) $(OBJ) $(INC) -Wl,--whole-archive $(LIB) -Wl,--no-whole-archive
	ln -s $(NAME) libft_malloc.so
clean:
	make clean -C $(LIB_DIR)
	rm -f $(OBJ)
fclean: clean
	make fclean -C $(LIB_DIR)
	rm -f $(NAME) libft_malloc.so
re: fclean all
