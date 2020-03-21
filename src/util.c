/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorzhak <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 12:57:19 by akorzhak          #+#    #+#             */
/*   Updated: 2020/03/01 12:57:20 by akorzhak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

void check()
{
//	printf("I'm in check()\n");
	t_page *page;
	t_block *block;

	if ((page = g_malloc_db[2]) == NULL) {
		printf("Your pointers doesn't work for pages\n");
		return;
	}
	while (page)
	{
		int i = 0;
		printf("Page allocated\n");
	//	printf("Page is free: %d\n", page->is_free);
		block = page->first_occupied_block;

		while (block) {
			
		//	printf("Block is free: %d\n", block->is_free);
			block = block->next;
			i++;
		}
		printf("num of blocks allocated, no: %d\n", i);
		page = page->next;
	}
}

e_zone get_page_zone_enum(size_t size)
{
	if (BLOCK_SIZE_TINY >= size)
		return TINY;
	else if (BLOCK_SIZE_SMALL >= size)
		return SMALL;
	else
		return LARGE;
}
