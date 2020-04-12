/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   occupy_free_block.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorzhak <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 12:57:19 by akorzhak          #+#    #+#             */
/*   Updated: 2020/03/01 12:57:20 by akorzhak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

t_block *find_matching_free_block_to_occupy(t_block *free_block, size_t size)
{
	printf("find_matching_free_block_to_occupy\n");

	while (free_block) {
		if (free_block->total_size >= size) //free blocks should be sorted
			return (free_block);
		free_block = free_block->next;
	}
	return (NULL);
}

t_block *try_to_occupy_free_block(t_page *page, size_t size) //TODO REDO
{
	printf("TRY_TO_OCCUPY_FREE_BLOCK\n");
	t_block *free_block_to_occupy;

	if (!page->first_free_block)
		return (NULL);
	if ((free_block_to_occupy =
		find_matching_free_block_to_occupy(page->first_free_block, size)) == NULL)
		return (NULL);	
	if (page->first_occupied_block)
		page->first_occupied_block->prev = free_block_to_occupy;
	if (page->first_free_block == free_block_to_occupy) {
		page->first_free_block = free_block_to_occupy->next;  //WRONG, what if free block is in the middle?
		if (page->first_free_block)
			page->first_free_block->prev = NULL;
	}
	free_block_to_occupy->next = page->first_occupied_block;
	free_block_to_occupy->prev = NULL;
	free_block_to_occupy->allocated_size = size;
	page->first_occupied_block = free_block_to_occupy;
	return (free_block_to_occupy);
}
