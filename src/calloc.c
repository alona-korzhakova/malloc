/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   calloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorzhak <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 12:57:19 by akorzhak          #+#    #+#             */
/*   Updated: 2020/03/01 12:57:20 by akorzhak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

void	*calloc_internal(size_t nmemb, size_t size)
{
	printf("CALLOC_internal\n");
	void *ptr;

	if ((ptr = malloc_internal(nmemb * size)) == NULL) {
		printf("Not ok...malloc\n");
		return (0);
	}
	ft_bzero(ptr, nmemb * size);
	return (ptr);
}

void 	*s_calloc(size_t nmemb, size_t size)
{
	printf("S_CALLOC\n");
	void *new_ptr;

	pthread_mutex_lock(&g_mutex_malloc);
	new_ptr = calloc_internal(nmemb, size);
	pthread_mutex_unlock(&g_mutex_malloc);
	return (new_ptr);
}
