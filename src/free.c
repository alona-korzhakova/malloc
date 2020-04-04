/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorzhak <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 12:57:19 by akorzhak          #+#    #+#             */
/*   Updated: 2020/03/01 12:57:20 by akorzhak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

void	insert_block_to_asc_sorted_free_block_db(t_block *block_to_free, t_block *free_block)
{
	while (free_block->next) {
		if (free_block->reserved_size >= block_to_free->reserved_size) {
			block_to_free->next = free_block;
			block_to_free->prev = free_block->prev;
			free_block->prev = block_to_free;
			if (block_to_free->prev)
				block_to_free->prev->next = block_to_free;
			return;
		}
		free_block = free_block->next;
	}
	free_block->next = block_to_free;
	block_to_free->prev = free_block;
	block_to_free->next = NULL;
}
void	free_block(t_page *page, void *ptr)
{
	printf("FREE_BLOCK\n");
	t_block *block_to_free;

	block_to_free = (t_block *)(ptr - SIZE_OF_BLOCK_METADATA);
	
	if (page->first_occupied_block == block_to_free) {
		printf("FREE_BLOCK: first_occupied_block is being freed\n");
		page->first_occupied_block = block_to_free->next;
		if (page->first_occupied_block)
			page->first_occupied_block->prev = NULL;
	}
	//restore occupied block list
	if (block_to_free->prev)
		block_to_free->prev->next = block_to_free->next;
	if (block_to_free->next)
		block_to_free->next->prev = block_to_free->prev;
	if (!page->first_free_block) {
		printf("FREE_BLOCK: first_free_block is now added\n");
		page->first_free_block = block_to_free;
		block_to_free->prev = NULL;
		block_to_free->next = NULL;
	}
	else {
		printf("FREE_BLOCK: free_block is now added to the list\n");
		insert_block_to_asc_sorted_free_block_db(block_to_free, page->first_free_block);
	}	
	page->is_free = TRUE;
}

void	free_large_page(t_page *page_to_free)
{
	printf("FREE_LARGE_PAGE\n");

	if (g_malloc_db[LARGE] == page_to_free)
		g_malloc_db[LARGE] = page_to_free->next;
	if (page_to_free->prev)
		page_to_free->prev->next = page_to_free->next;
	if (page_to_free->next)
		page_to_free->next->prev = page_to_free->prev;
	munmap(page_to_free, (page_to_free->reserved_size + SIZE_OF_PAGE_METADATA));
}

void		free_internal(void *ptr)
{
	printf("FT_FREE\n");
	e_zone 	page_zone;
	t_page	*page;

	if (!ptr)
		return;
	page_zone = TINY;
	if ((page = find_page_with_ptr(ptr, &page_zone)) == NULL)
		return;
	if (LARGE == page_zone)
		return free_large_page(page);
	else
		return free_block(page, ptr);
}

void	s_free(void *ptr)
{
	printf("S_FREE\n");
	pthread_mutex_lock(&g_mutex_malloc);
	free_internal(ptr);
	pthread_mutex_unlock(&g_mutex_malloc);
}
