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

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define PAGE_ZONE_NUM				3
#define BLOCK_SIZE_TINY				128
#define BLOCK_SIZE_SMALL			1024
#define SIZE_OF_PAGE_HEADER 		sizeof(t_page)
#define SIZE_OF_BLOCK_HEADER 		sizeof(t_block)
#define MIN_NUM_OF_BLOCKS_PER_PAGE 	100
#define SIZE_OF_BLOCK_BODY(page_zone) ((page_zone) == TINY ? BLOCK_SIZE_TINY : BLOCK_SIZE_SMALL)


enum page_zone
{
	TINY,
	SMALL,
	LARGE
};

typedef struct 		s_block
{
	struct s_block	*next;
	size_t			size;
	char			is_free;
}					t_block;

typedef struct 		s_page
{
	struct s_page	*next;
	struct s_block	*block;
	char			is_free;
} 					t_page;

extern t_page			*g_page_zones[PAGE_ZONE_NUM];
extern pthread_mutex_t	g_mutex_malloc;

void	free(void *ptr);
void	*malloc(size_t size);
void	*realloc(void *ptr, size_t size);
void	show_alloc_mem(void);
void	show_alloc_mem_ex(void);

#endif
