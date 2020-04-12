/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_new_item.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorzhak <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 12:57:19 by akorzhak          #+#    #+#             */
/*   Updated: 2020/03/01 12:57:20 by akorzhak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

t_block *create_new_block(t_page *page, size_t size)
{
	printf("CREATE_NEW_BLOCK\n");
	t_block *new_block;
	t_block *block;

	block = page->first_occupied_block;
	if (block) {  ///add to the 1st pos
		while (block && block->next) {
			block = block->next;
		}
		new_block = (void *)block + SIZE_OF_BLOCK_METADATA + block->total_size;	
		block->next = new_block;
		new_block->prev = block;
	} else {
		new_block = (void*)page + SIZE_OF_PAGE_METADATA;
		page->first_occupied_block = new_block;
		new_block->prev = NULL;
	}
	new_block->next = NULL;
	new_block->allocated_size = size;
	new_block->total_size = size;
	if (size % ALIGNMENT)
		new_block->total_size = size + ALIGNMENT - (size % ALIGNMENT);
	return (new_block);
}

void	add_new_page_to_db(t_page *new_page, e_zone page_zone)
{
	printf("ADD_NEW_PAGE_TO_DB\n");
	if (g_malloc_db[page_zone] == NULL) {
		printf("ADD_NEW_PAGE_TO_DB: new_page is first in db\n");
		g_malloc_db[page_zone] = new_page;
	} else {
		printf("ADD_NEW_PAGE_TO_DB: new_page is NOT first in db\n");
		new_page->next = g_malloc_db[page_zone];
		new_page->next->prev = new_page;
		g_malloc_db[page_zone] = new_page;
	}
}

t_page *create_new_page(e_zone page_zone, size_t requested_data_size)
{
	printf("CREATE_NEW_PAGE\n");
	t_page *new_page;
	size_t row_size;
	size_t aligned_size;

	if (LARGE == page_zone)
		row_size = SIZE_OF_PAGE_METADATA + requested_data_size;
	else {
		row_size = SIZE_OF_PAGE_METADATA + (SIZE_OF_BLOCK_METADATA +
					SIZE_OF_BLOCK_BODY(page_zone)) * INITIAL_NUM_OF_BLOCKS_PER_PAGE;
	}
	aligned_size = row_size;
	if (row_size % getpagesize())
		aligned_size = row_size + getpagesize() - row_size % getpagesize();
	new_page = (t_page*)mmap(0, aligned_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (MAP_FAILED == new_page)
		return (NULL);
	new_page_init(new_page, aligned_size);
	if (LARGE == page_zone) {
		new_page->allocated_size = requested_data_size;
		new_page->is_free = FALSE;
	}	
	return (new_page);
}
