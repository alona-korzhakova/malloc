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

void check()
{
//	printf("I'm in check()\n");
	t_page *page;
	t_block *block;

	if ((page = g_page_zones[1]) == NULL) {
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

t_block *occupy_free_block(t_page *page, size_t size)
{
	t_block *free_block_to_occupy;

	free_block_to_occupy = page->first_free_block;

	page->first_occupied_block->prev = free_block_to_occupy;
	page->first_free_block           = free_block_to_occupy->next;
	free_block_to_occupy->next       = page->first_occupied_block;
	page->first_occupied_block       = free_block_to_occupy;
	page->first_occupied_block->prev = NULL;
	page->first_free_block->prev     = NULL;

	return (free_block_to_occupy);
}

t_block *create_new_block(t_page *page, size_t size, size_t default_block_size)
{
	//int tmp = 0;
	t_block *new_block;
	t_block *block;
//	printf("Creating new block\n");
	// printf("Address of original page is: %p\n", (void *)g_page_zones[0]);
	// printf("Address of passed page is: %p\n", (void *)page);
	block = page->first_occupied_block;
	if (block) {
		// printf("Last occupied block pointer is not null\n");
		// printf("Address of page is: %p\n", (void *)page);
		// printf("Size of page header is: %lx\n", SIZE_OF_PAGE_HEADER);

		while (block && block->next) {
		//	tmp++;
		//	printf("occupied blk: %d\n", tmp);
			block = block->next;
		}
		new_block = (void *)block + SIZE_OF_BLOCK_HEADER + default_block_size;
		//printf("Address of new block is: %p\n", (void *)new_block);
		// printf("Address of previous block is: %p\n", (void *)page_ptr->first_occupied_block);
		// printf("Size of block header: %ld\n", SIZE_OF_BLOCK_HEADER);
		// printf("Default block size: %ld\n", default_block_size);
		
		block->next = new_block;
		new_block->prev = block;
		// printf("Address of page_ptr->first_occupied_block->next: %p\n", page_ptr->first_occupied_block->next);
		// printf("Address of new block with void*: %p\n", (void *)new_block);
		//while
		// printf("Address of new last occupied block is: %p\n", (void *)page_ptr->last_first_occupied_block->next);
		// printf("Address of new last occupied block in global var is: %p\n", (void *)g_page_ptr_zones[0]->last_first_occupied_block->next);

	} else {
		// printf("Address of page_ptr is: %p\n", (void *)page_ptr);
		// printf("Size of page_ptr header is: %lx\n", SIZE_OF_PAGE_ptr_HEADER);
		new_block = (void*)page + SIZE_OF_PAGE_HEADER;
		//printf("Address of new block is: %p\n", (void *)new_block);
		page->first_occupied_block = new_block;
		new_block->prev = NULL;
	}
	new_block->next = NULL;
	new_block->size = size;
	// printf("Address of new block: %p\n", (void *)new_block);
	// printf("Address of page->first_occupied_block: %p\n", (void *)page->first_occupied_block);
	// printf("Address of next block after new created is: %p\n", (void *)new_block + (SIZE_OF_BLOCK_HEADER + default_block_size) * 2);
	// printf("Address of next page is: %p\n", (void *)page + page->size);
	// printf("Address of page is: %p\n", (void *)page);
	return (new_block);
}

void new_page_init(t_page *new_page, size_t size)
{
	new_page->next = NULL;
	new_page->prev = NULL;
	new_page->first_occupied_block = NULL;
	new_page->total_size = size - SIZE_OF_PAGE_HEADER;
	new_page->occupied_size = 0;
	new_page->is_free = 1;
}

t_page *create_new_page(e_zone page_zone, size_t requested_data_size)
{
	t_page *page;
	t_page *new_page;
	size_t row_size;
	size_t aligned_size;

	if (LARGE == page_zone)
		row_size = SIZE_OF_PAGE_HEADER + requested_data_size;
	else{
		row_size = SIZE_OF_PAGE_HEADER + (SIZE_OF_BLOCK_HEADER + SIZE_OF_BLOCK_BODY(page_zone)) * INITIAL_NUM_OF_BLOCKS_PER_PAGE;
	}
	aligned_size = row_size + getpagesize() - row_size % getpagesize();
	new_page = (t_page*)mmap(0, aligned_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (MAP_FAILED == new_page)
		return (NULL);
	new_page_init(new_page, aligned_size);
	if (LARGE == page_zone) {
		new_page->occupied_size = requested_data_size;
		new_page->is_free = 0;
	}
	if ((page = g_page_zones[page_zone]) == NULL) {
		g_page_zones[page_zone] = new_page;
	} else {
		while (page && page->next)
			page = page->next;
		page->next = new_page;
		new_page->prev = page;
	}
	return (new_page);
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

void	*ft_malloc(size_t size)
{
	t_page	*page;
	t_block *new_block;
	e_zone page_zone;

	if (!size)
		return NULL;
	page_zone = get_page_zone_enum(size);
	page = g_page_zones[page_zone];
	while (page && !page->is_free)
		page = page->next;
	if (!page) {
		if ((page = create_new_page(page_zone, size)) == NULL)
			return NULL;
		if (LARGE == page_zone)
			return ((void *)page + SIZE_OF_PAGE_HEADER);
	}
	new_block = (page->first_free_block) ?
				occupy_free_block(page, size) :
				create_new_block(page, size, SIZE_OF_BLOCK_BODY(page_zone));

	void *possible_next_block_ptr       = (void *)new_block + SIZE_OF_BLOCK_HEADER + SIZE_OF_BLOCK_BODY(page_zone);
	void *valid_start_of_last_block_ptr = (void *)page + SIZE_OF_PAGE_HEADER + page->total_size -
										(SIZE_OF_BLOCK_HEADER + SIZE_OF_BLOCK_BODY(page_zone));
	if (possible_next_block_ptr >= valid_start_of_last_block_ptr)
	{
		printf("Page is not free now\n");
		page->is_free = 0;
	}
	// printf("return address is: %p\n", (void *)new_block + SIZE_OF_BLOCK_HEADER);
	return ((void *)new_block + SIZE_OF_BLOCK_HEADER);
}

void 	*s_malloc(size_t size)
{
	//printf("I'm in your malloc\n");
	void *new_ptr;

	pthread_mutex_lock(&g_mutex_malloc);
	new_ptr = ft_malloc(size);
	pthread_mutex_unlock(&g_mutex_malloc);
	return (new_ptr);
}

int 	block_exists(void *ptr, e_zone page_zone)
{
	t_page *page;
	t_block *block;

	page = g_page_zones[page_zone];

	while (page) {
		if ((((void *)page + SIZE_OF_PAGE_HEADER) <= ptr) && 
			(((void *)page + SIZE_OF_PAGE_HEADER + page->total_size) >= ptr)) {
			break;
		}
		page = page->next;
	}
	if (!page)
		return (FALSE);
	block = page->first_occupied_block;
	while (block) {
		if (ptr == (void *)block)
			break;
		block = block->next;
	}
	if (!block) {
		return (FALSE);
		// *block_occupied_size = block->size;
		// *block_total_size = SIZE_OF_BLOCK_BODY(page_zone);
	}
	return (TRUE);
}

int page_in_large_zone_exists(void *ptr)
{
	t_page *page;

	page = g_page_zones[LARGE];
	while (page) {
		if (((void *)page + SIZE_OF_PAGE_HEADER) == ptr)
			break;
		page = page->next;
	}
	if (!page)
		return (FALSE);
	return (TRUE);
}

int is_ptr_valid(void *ptr, e_zone *page_zone)
{
	if (block_exists(ptr, TINY)) {
		*page_zone = TINY;
		return (TRUE);
	}
	if (block_exists(ptr, SMALL)) {
		*page_zone = SMALL;
		return (TRUE);
	}
	if (page_in_large_zone_exists(ptr)) {
		*page_zone = LARGE;
		return (TRUE);
	}
	return (FALSE);
}

void	ft_free_block(void *ptr, e_zone page_zone)
{
	t_block *block_to_free;

	block_to_free = (t_block *)(ptr - SIZE_OF_BLOCK_HEADER);
	if (g_page_zones[page_zone]->first_occupied_block == block_to_free) {
		g_page_zones[page_zone]->first_occupied_block = block_to_free->next;
		g_page_zones[page_zone]->first_occupied_block->prev = NULL;
	} else {
		block_to_free->prev->next = block_to_free->next;
		block_to_free->next->prev = block_to_free->prev;
	}
}

void	ft_free_large_page(void *ptr)
{
	t_page *page_to_free;

	page_to_free = (t_page *)(ptr - SIZE_OF_PAGE_HEADER);
	if (g_page_zones[LARGE] == page_to_free) {
		g_page_zones[LARGE] = page_to_free->next;
		g_page_zones[LARGE]->prev = NULL; //is needed????
	} else {
		// while (page && page->next)
		// {
		// 	if (page->next == page_to_free) {
		// 		page->next = page_to_free->next;			
		// 		break;
		// 	}
		// 	page = page->next;
		// }
		page_to_free->prev->next = page_to_free->next;
		page_to_free->next->prev = page_to_free->prev;
	}
	munmap(page_to_free, (page_to_free->total_size + SIZE_OF_PAGE_HEADER));
}

void	*ft_realloc_large_page(void *old_mem_ptr, size_t new_size)
{
	t_page *page;
	t_page *old_page;
	t_page *new_page;
	void   *new_mem_ptr;

	old_page = (t_page *)(old_mem_ptr - SIZE_OF_PAGE_HEADER);	
	if (new_size <= old_page->total_size) {
		old_page->occupied_size = new_size;
		return old_mem_ptr;
	}
	if ((new_mem_ptr = ft_malloc(new_size)) == NULL)
		return NULL;
	memcpy(new_mem_ptr, old_mem_ptr, old_page->occupied_size);
	new_page = (t_page *)(new_mem_ptr - SIZE_OF_PAGE_HEADER);
	
	if (g_page_zones[LARGE] == old_page) {
		g_page_zones[LARGE]->prev = new_page;
		new_page->next = g_page_zones[LARGE];
		g_page_zones[LARGE] = new_page;
	}
	else {
		page = g_page_zones[LARGE];
		while (page && page->next)
		{
			if (page->next == old_page) {
				new_page->next = old_page->next;
				//ft_free_page(old_page);  //TODO				
				page->next = new_page;
				break;
			}
			page = page->next;
		}
	}
	ft_free_large_page(old_page);
	return (new_mem_ptr);	
}

void		ft_free(void *ptr)
{
	e_zone 	page_zone;

	if (!ptr)
		return;
	page_zone = TINY;
	if (is_ptr_valid(ptr, &page_zone) == FALSE)
		return;
	if (LARGE == page_zone)
		return ft_free_large_page(ptr);
	else
		return ft_free_block(ptr, page_zone);

	// res = first_free_block(&g_malloc_zones.tiny, ptr);
	// if (res == 0)
	// 	res = first_free_block(&g_malloc_zones.small, ptr);
	// if (res == 0)
	// 	free_large(ptr);
}

void	s_free(void *ptr)
{
	pthread_mutex_lock(&g_mutex_malloc);
	ft_free(ptr);
	pthread_mutex_unlock(&g_mutex_malloc);
}

void	*s_realloc(void *ptr, size_t size)
{
	void *new_ptr;

	pthread_mutex_lock(&g_mutex_malloc);
	new_ptr = ft_realloc(ptr, size);
	pthread_mutex_unlock(&g_mutex_malloc);
	return (new_ptr);
}

void	*ft_realloc(void *ptr, size_t size)
{
	e_zone 	page_zone;

	if (!ptr)
		return (s_malloc(size));
	if (!size)
	{
		s_free(ptr);
	}
	page_zone = TINY;
	if (is_ptr_valid(ptr, &page_zone) == FALSE)
		return (NULL);
	if (LARGE == page_zone)
		return (ft_realloc_large_page(ptr, size));
	else
		//return (ft_realloc_tiny_small(ptr, old_used_size, old_total_size, size));
		return NULL;
}