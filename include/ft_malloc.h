/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorzhak <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 12:56:50 by akorzhak          #+#    #+#             */
/*   Updated: 2020/03/01 12:56:56 by akorzhak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MALLOC_H
# define FT_MALLOC_H

#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#define PAGE_ZONE_NUM					3
#define BLOCK_SIZE_TINY					128
#define BLOCK_SIZE_SMALL				1024
#define ALIGNMENT						(sizeof(long double))
#define SIZE_OF_PAGE_HEADER 			(sizeof(t_page))
#define SIZE_OF_BLOCK_HEADER 			(sizeof(t_block))
#define SIZE_OF_PAGE_METADATA			(SIZE_OF_PAGE_HEADER + (SIZE_OF_PAGE_HEADER % ALIGNMENT))
#define SIZE_OF_BLOCK_METADATA			(SIZE_OF_BLOCK_HEADER + (SIZE_OF_BLOCK_HEADER % ALIGNMENT))
#define INITIAL_NUM_OF_BLOCKS_PER_PAGE 	100
#define SIZE_OF_BLOCK_BODY(page_zone) ((page_zone) == TINY ? BLOCK_SIZE_TINY : BLOCK_SIZE_SMALL)
#define TRUE							1
#define FALSE							0
#define MIN(first, second)				((first) <= (second) ? (first) : (second))


typedef enum page_zone
{
	TINY,
	SMALL,
	LARGE
} e_zone;

typedef struct 		s_block
{
	struct s_block	*next;
	struct s_block	*prev;
	size_t 			total_size;
	size_t			occupied_size;
	int				is_free;
}					t_block;

typedef struct 		s_page
{
	struct s_page	*next;
	struct s_page	*prev;
	struct s_block	*first_occupied_block;
	struct s_block	*first_free_block;
	size_t			total_size;
	size_t			occupied_size;
	int				is_free;
} 					t_page;

extern t_page			*g_malloc_db[PAGE_ZONE_NUM];
extern pthread_mutex_t	g_mutex_malloc;

void	s_free(void *ptr);
void	*s_malloc(size_t size);
void	*s_realloc(void *ptr, size_t size);
void	show_alloc_mem(void);
void	show_alloc_mem_ex(void);

void	check();
e_zone 	get_page_zone_enum(size_t size);
t_page 	*find_page_with_ptr(void *ptr, e_zone *page_zone);
t_block *try_to_occupy_free_block(t_page *page, size_t size);
void 	new_page_init(t_page *new_page, size_t size);
void	free_block(t_page *page, void *ptr);
void	free_large_page(t_page *page_to_free);
t_page *create_new_page(e_zone page_zone, size_t requested_data_size);
void	add_new_page_to_db(t_page *new_page, e_zone page_zone);
t_block *create_new_block(t_page *page, size_t size);

#endif
