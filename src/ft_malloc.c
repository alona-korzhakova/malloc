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

enum page_zone get_page_zone(size_t size)
{
	if (MAX_BLOCK_SIZE_TINY >= size)
		return TINY;
	else if (MAX_BLOCK_SIZE_SMALL >= size)
		return SMALL;
	else
		return LARGE;
}

void	*ft_malloc(size_t size)
{
	t_block *new_block_ptr;
	enum page_zone page_zone = get_page_zone(size);

	while (g_page_zones[page_zone])
	{
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
