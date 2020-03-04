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

void occupy_free_block(t_page *page, size_t size)
{
	page->last_occupied_block->next = page->free_block;
	page->last_occupied_block = page->free_block;
	page->last_occupied_block->size = size;
	page->last_occupied_block->is_free = 0;
	page->free_block = NULL;
}

t_block *create_new_block(t_page *page, size_t size)
{
	t_block *new_block;

	printf("Creating new block\n");
	if (page->last_occupied_block) {
		new_block = (void*)page->last_occupied_block + SIZE_OF_BLOCK_HEADER + page->last_occupied_block->size + 1;
		page->last_occupied_block->next = new_block;

	} else {
		printf("Address of page is: %p\n", (void *)page);
		printf("Size of page header is: %lx\n", SIZE_OF_PAGE_HEADER);
		new_block = (void*)page + SIZE_OF_PAGE_HEADER + 1;
		printf("Address of new block is: %p\n", (void *)new_block);
		page->first_occupied_block = new_block;
	}
	page->last_occupied_block = new_block;
	page->last_occupied_block->size = size;
	page->last_occupied_block->is_free = 0;
	return (new_block);
}

t_page *create_new_page(enum page_zone page_zone, size_t requested_block_size)
{
	printf("Creating new page\n");
	t_page *page;
	t_page *new_page;
	size_t row_size;
	size_t aligned_size;

	if (LARGE == page_zone)
		row_size = SIZE_OF_PAGE_HEADER + SIZE_OF_BLOCK_HEADER + requested_block_size;
	else{
		printf("Not large page zone\n");
		row_size = SIZE_OF_PAGE_HEADER + (SIZE_OF_BLOCK_HEADER + SIZE_OF_BLOCK_BODY(page_zone)) * INITIAL_NUM_OF_BLOCKS_PER_PAGE;
		printf("row_size: %ld\n", row_size);
	}
	aligned_size = row_size + getpagesize() - row_size % getpagesize();
	printf("aligned_size: %ld\n", aligned_size);
	new_page = (t_page*)mmap(0, aligned_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (MAP_FAILED == new_page)
		return (NULL);
	if ((page = g_page_zones[page_zone]) == NULL) {
		printf("Adding first page to global array\n");
		page = new_page;
		page->next = NULL;
	} else {
		while (page && page->next)
			page = page->next;
		page->next = new_page;
	}
	new_page->is_free = 1;
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
	t_block *free_block;
	t_page	*page;
	enum page_zone page_zone = get_page_zone_enum(size);

	page = g_page_zones[page_zone];
	while (page && !page->is_free)
		page = page->next;
	if (!page) {
		if ((page = create_new_page(page_zone, size)) == NULL)
			return NULL;
	}
	free_block = page->free_block;
	if (!free_block) {
		if ((free_block = create_new_block(page, size)) == NULL)
			return NULL;
	}
	else {
		occupy_free_block(page, size);
	}
	return (free_block + SIZE_OF_BLOCK_HEADER);
}

void 	*s_malloc(size_t size)
{
	printf("I'm in your malloc\n");
	void *new_block_ptr;

	pthread_mutex_lock(&g_mutex_malloc);
	new_block_ptr = ft_malloc(size);
	pthread_mutex_unlock(&g_mutex_malloc);
	return (new_block_ptr);
}
