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
t_page			*g_malloc_db[PAGE_ZONE_NUM] = {NULL};

void	*malloc_internal(size_t size)
{
	printf("MALLOC_internal\n");

	t_page	*page;
	t_block *new_block;
	e_zone page_zone;

	if (!size)
		return (NULL);
	page_zone = get_page_zone_enum(size);
	page = g_malloc_db[page_zone];
	if (LARGE != page_zone) {
		while (page && !page->is_free)
			page = page->next;
	}
	if ((LARGE == page_zone) || !page) {
		printf("MALLOC_internal: no free page found\n");
		if ((page = create_new_page(page_zone, size)) == NULL)
			return (NULL);
		add_new_page_to_db(page, page_zone);
		if (LARGE == page_zone)
			return ((void *)page + SIZE_OF_PAGE_METADATA);
	}
	if ((new_block = try_to_occupy_free_block(page, size)) == NULL)
		new_block = create_new_block(page, size);

	void *possible_next_block_ptr       = (void *)new_block + SIZE_OF_BLOCK_METADATA + SIZE_OF_BLOCK_BODY(page_zone);
	void *valid_start_of_last_block_ptr = (void *)page + SIZE_OF_PAGE_METADATA + page->reserved_size -
										(SIZE_OF_BLOCK_METADATA + SIZE_OF_BLOCK_BODY(page_zone));
	if (possible_next_block_ptr >= valid_start_of_last_block_ptr)
	{
		printf("MALLOC_internal: Page is not free now\n");
		page->is_free = FALSE;
	}
	printf("return address new_block: %p\n", (void *)new_block);
	printf("SIZE_OF_BLOCK_METADATA: %ld\n", SIZE_OF_BLOCK_METADATA);
	printf("return address of malloc block: %p\n", (void *)new_block + SIZE_OF_BLOCK_METADATA);
	return ((void *)new_block + SIZE_OF_BLOCK_METADATA);
}

void 	*s_malloc(size_t size)
{
	printf("S_MALLOC\n");
	void *new_ptr;

	pthread_mutex_lock(&g_mutex_malloc);
	new_ptr = malloc_internal(size);
	pthread_mutex_unlock(&g_mutex_malloc);
	return (new_ptr);
}
