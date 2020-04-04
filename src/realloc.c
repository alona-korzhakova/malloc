 /* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   m_realloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorzhak <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 12:57:19 by akorzhak          #+#    #+#             */
/*   Updated: 2020/03/01 12:57:20 by akorzhak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

void	*realloc_block(t_page *page, e_zone old_page_zone, void *old_mem_ptr, size_t new_size)
{
	printf("REALLOC_BLOCK\n");
	t_block *old_block;
	e_zone	new_page_zone;
	void   *new_mem_ptr;

	old_block = (t_block *)(old_mem_ptr - SIZE_OF_BLOCK_METADATA);
	printf("old_block size: %ld\n", old_block->allocated_size);
	printf("new size: %ld\n", new_size);
	new_page_zone = get_page_zone_enum(new_size);
	if ((new_page_zone == old_page_zone) &&
		(new_size <= old_block->reserved_size)) ///size is default block size REDO //old_block->allocated_size
	{
		printf("REALLOC_BLOCK: NEW SIZE REQUESTED IS LESS THEN PREVIOUS, RETURNING THE SAME PTR\n");
		//split the rest?
		old_block->allocated_size = new_size;
		return (old_mem_ptr);
	}
	if ((new_mem_ptr = malloc_internal(new_size)) == NULL)
		return NULL;
	ft_memcpy(new_mem_ptr, old_mem_ptr, MIN(new_size, old_block->allocated_size));	//will copy with extra bytes, redo
	free_block(page, old_mem_ptr);
	return (new_mem_ptr);
}

void	*realloc_large_page(void *old_mem_ptr, size_t new_size)
{
	printf("REALLOC_LARGE_PAGE\n");

	t_page	*old_page;
	void	*new_mem_ptr;
	e_zone	page_zone_for_new_size;
	size_t	aligned_new_size;
	
	old_page = (t_page *)(old_mem_ptr - SIZE_OF_PAGE_METADATA);
	page_zone_for_new_size = get_page_zone_enum(new_size);

	if (LARGE == page_zone_for_new_size) { //is there is a sense in substracting page header from total size?
		aligned_new_size = (SIZE_OF_PAGE_METADATA + new_size) + getpagesize() -
							(SIZE_OF_PAGE_METADATA + new_size) % getpagesize();
		if (aligned_new_size == (old_page->reserved_size + SIZE_OF_PAGE_METADATA)) {
			printf("REALLOC_LARGE_PAGE: NEW SIZE REQUESTED WILL GIVE SAME SIZE TO MALLOC, RETURNING THE SAME PTR\n");
			old_page->allocated_size = new_size;
			return old_mem_ptr;
		}
	}
	printf("REALLOC_LARGE_PAGE: g_malloc_db[LARGE] before new_page creation is: %p\n", g_malloc_db[LARGE]);
	if ((new_mem_ptr = malloc_internal(new_size)) == NULL)
		return NULL;
	printf("REALLOC_LARGE_PAGE: g_malloc_db[LARGE] after new_page creation is: %p\n", g_malloc_db[LARGE]);
	memcpy(new_mem_ptr, old_mem_ptr, MIN(new_size, old_page->allocated_size));
	if (LARGE == page_zone_for_new_size)
		printf("REALLOC_LARGE_PAGE: new_page address is: %p\n", new_mem_ptr - SIZE_OF_PAGE_METADATA);
	else
		printf("REALLOC_LARGE_PAGE: new_page address is: %p\n", new_mem_ptr - SIZE_OF_BLOCK_METADATA);
	printf("REALLOC_LARGE_PAGE: old_page address is: %p\n", (void *)old_page);	
	free_large_page(old_page);
	return (new_mem_ptr);
}

void	*realloc_internal(void *ptr, size_t size)
{
	printf("m_REALLOC\n");
	e_zone 	page_zone;
	t_page	*page;

	if (!ptr)
		return (malloc_internal(size));
	printf("size: %ld\n", size);
	if (!size)
	{
		free_internal(ptr);
		return NULL;
	}
	page_zone = TINY;
	if ((page = find_page_with_ptr(ptr, &page_zone)) == NULL) {
		printf("m_REALLOC: PTR IS NOT VALID\n");
		return (NULL);
	}
	if (LARGE == page_zone) {
		return (realloc_large_page(ptr, size));
	}
	else
		return (realloc_block(page, page_zone, ptr, size));
}

void	*s_realloc(void *ptr, size_t size)
{
	printf("S_REALLOC\n");
	void *new_ptr;

	pthread_mutex_lock(&g_mutex_malloc);
	new_ptr = realloc_internal(ptr, size);
	pthread_mutex_unlock(&g_mutex_malloc);
	return (new_ptr);
}
