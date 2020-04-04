/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorzhak <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 12:57:19 by akorzhak          #+#    #+#             */
/*   Updated: 2020/03/01 12:57:20 by akorzhak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

void new_page_init(t_page *new_page, size_t size)
{
	printf("NEW PAGE INIT\n");
	new_page->next = NULL;
	new_page->prev = NULL;
	new_page->first_occupied_block = NULL;
	new_page->reserved_size = size - SIZE_OF_PAGE_METADATA;
	new_page->allocated_size = 0;
	new_page->is_free = TRUE;
}
