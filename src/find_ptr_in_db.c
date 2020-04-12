/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_ptr_in_db.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorzhak <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 12:57:19 by akorzhak          #+#    #+#             */
/*   Updated: 2020/03/01 12:57:20 by akorzhak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

t_page	*find_page_with_block(void *ptr, e_zone page_zone)
{
	t_page *page;
	t_block *block;
	t_block *block_to_find;

	if ((page = g_malloc_db[page_zone]) == NULL)///do i need page_zone???
		return (NULL);
	block_to_find = (t_block *)(ptr - SIZE_OF_BLOCK_METADATA);
	printf("Address of page start: %p\n", (void *)page + SIZE_OF_PAGE_METADATA);
	printf("Address of page end: %p\n", (void *)page + SIZE_OF_PAGE_METADATA + page->total_size);
	printf("Address of ptr: %p\n", ptr);
	printf("page total_size: %ld\n", page->total_size); 
	while (page) {
		if ((((void *)page + SIZE_OF_PAGE_METADATA) <= (void *)block_to_find) && 
			(((void *)page + SIZE_OF_PAGE_METADATA + page->total_size) >= (void *)block_to_find)) {
			break;
		}
		page = page->next;
	}
	if (page) {
		printf("page is found\n");
		block = page->first_occupied_block;
		printf("address of block: %p\n", page->first_occupied_block);
		while (block) {
			printf("address of block: %p\n", page->first_occupied_block);
			printf("address of block to find: %p\n", block_to_find);
			if (block == block_to_find)
				break;
			block = block->next;
		}
		if (!block)
			return (NULL);
	}
	return (page);
}

int page_in_large_zone_exists(void *ptr)
{
	t_page *page;

	page = g_malloc_db[LARGE];
	while (page) {
		if (((void *)page + SIZE_OF_PAGE_METADATA) == ptr)
			break;
		page = page->next;
	}
	if (!page)
		return (FALSE);
	return (TRUE);
}

t_page *find_page_with_ptr(void *ptr, e_zone *page_zone)
{
	printf("IS_PTR_VALID\n");
	t_page *page;
	if ((page = find_page_with_block(ptr, TINY)) != NULL) {
		*page_zone = TINY;
		printf("IS_PTR_VALID: BLOCK IS FOUND IN TINY\n");
		return (page);
	}
	if ((page = find_page_with_block(ptr, SMALL)) != NULL) {
		*page_zone = SMALL;
		printf("IS_PTR_VALID: BLOCK IS FOUND IN SMALL\n");
		return (page);
	}
	if (page_in_large_zone_exists(ptr) == TRUE) {
		*page_zone = LARGE;
		printf("IS_PTR_VALID: BLOCK IS FOUND IN LARGE\n");
		return (ptr);
	}
	printf("IS_PTR_VALID: BLOCK IS NOT FOUND\n");
	return (NULL);
}
