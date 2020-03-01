/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorzhak <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 12:57:19 by akorzhak          #+#    #+#             */
/*   Updated: 2020/03/01 12:57:20 by akorzhak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

pthread_mutex_t g_mutex_malloc = PTHREAD_MUTEX_INITIALIZER;
t_page			*g_page_zones[PAGE_ZONE_NUM] = {NULL};

int create_new_page(enum page_zone, size_t requested_block_size)
{
	void *new_page;
	size_t row_size;
	size_t aligned_size;

	if (LARGE = page_zone)
		row_size = SIZE_OF_PAGE_HEADER + SIZE_OF_BLOCK_HEADER + requested_block_size;
	else
		row_size = SIZE_OF_PAGE_HEADER + (SIZE_OF_BLOCK_HEADER + SIZE_OF_BLOCK_BODY(page_zone)) * MIN_NUM_OF_BLOCKS_PER_PAGE;
	aligned_size = row_size + getpagesize() - row_size % getpagesize();
	new_page = mmap(0, aligned_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (MAP_FAILED == new_page)
		return (NULL);
	return (new_page);
}

enum page_zone get_page_zone_enum(size_t size)
{
	if (BLOCK_SIZE_TINY >= size)
		return TINY;
	else if (BLOCK_SIZE_SMALL >= size)
		return SMALL;
	else
		return LARGE;
}

void	*ft_malloc(size_t size)
{
	t_block *block_ptr;
	t_page	*page;
	enum page_zone page_zone = get_page_zone_enum(size);

	page = g_page_zones[page_zone];
	while (page && !page->is_free)
	{
		page = page->next;
	}
	if (!page)
		page = create_new_page(page_zone);
	block_ptr = get_ptr_to_block(&page);

	while (g_page_zones[page_zone])
	{
		//block_ptr = get_ptr_to_block();
		g_page_zones[page_zone] = g_page_zones[page_zone]->next;
	}
	return NULL; //temp
}

void 	*malloc(size_t size)
{
	void *new_block_ptr;
	
	new_block_ptr = NULL;
	pthread_mutex_lock(&g_mutex_malloc);
	new_block_ptr = ft_malloc(size);
	pthread_mutex_unlock(&g_mutex_malloc);
	return (new_block_ptr);
}
