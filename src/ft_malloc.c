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
t_page			*g_zones_db[PAGE_ZONE_NUM] = {NULL};

void check()
{
//	printf("I'm in check()\n");
	t_page *page;
	t_block *block;

	if ((page = g_zones_db[2]) == NULL) {
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

t_block *find_matching_free_block_to_occupy(t_block *free_block, size_t size)
{
	printf("find_matching_free_block_to_occupy\n");
	t_block *best_match;
	size_t	size_difference;

	best_match = NULL;
	size_difference = 0;
	while (free_block) {
		if (free_block->total_size >= size) {
			if (best_match)
			{
				if (size_difference >= (free_block->total_size - size))
				{
					best_match = free_block;
					size_difference = free_block->total_size - size;
				}
			}
			else
			{
				best_match = free_block;
			}
		}
		free_block = free_block->next;
	}
	return (best_match);
}

t_block *try_to_occupy_free_block(t_page *page, size_t size) //TODO REDO
{
	printf("TRY_TO_OCCUPY_FREE_BLOCK\n");
	t_block *free_block;
	t_block *free_block_to_occupy;

	if (!page->first_free_block)
		return (NULL);
	if ((free_block_to_occupy =
		find_matching_free_block_to_occupy(page->first_free_block, size)) == NULL)
		return (NULL);	
	if (page->first_occupied_block)
		page->first_occupied_block->prev = free_block_to_occupy;
	if (page->first_free_block == free_block_to_occupy)
		page->first_free_block = free_block_to_occupy->next;
	if (page->first_free_block)
		page->first_free_block->prev = NULL;
	free_block_to_occupy->next = page->first_occupied_block;
	free_block_to_occupy->prev = NULL;
	free_block_to_occupy->occupied_size = size;
	page->first_occupied_block = free_block_to_occupy;
	return (free_block_to_occupy);
}
/////////////////////////////////////////////////////////////////////////////////////
t_block *create_new_block(t_page *page, size_t size, size_t default_block_size)
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
	new_block->occupied_size = size;
	new_block->total_size = size + (size % ALIGNMENT);
	return (new_block);
}

void new_page_init(t_page *new_page, size_t size)
{
	printf("NEW PAGE INIT\n");
	new_page->next = NULL;
	new_page->prev = NULL;
	new_page->first_occupied_block = NULL;
	new_page->total_size = size - SIZE_OF_PAGE_METADATA;
	new_page->occupied_size = 0;
	new_page->is_free = TRUE;
}

void	add_new_page_to_db(t_page *new_page, e_zone page_zone)
{
	printf("ADD_NEW_PAGE_TO_DB\n");
	t_page *page = g_zones_db[page_zone];
	if (g_zones_db[page_zone] == NULL) {
		printf("ADD_NEW_PAGE_TO_DB: new_page is first in db\n");
		g_zones_db[page_zone] = new_page;
	} else {
		printf("ADD_NEW_PAGE_TO_DB: new_page is NOT first in db\n");
		new_page->next = g_zones_db[page_zone];
		new_page->next->prev = new_page;
		g_zones_db[page_zone] = new_page;
	}
}

t_page *create_new_page(e_zone page_zone, size_t requested_data_size)
{
	printf("CREATE_NEW_PAGE\n");
	t_page *page;
	t_page *new_page;
	size_t row_size;
	size_t aligned_size;

	if (LARGE == page_zone)
		row_size = SIZE_OF_PAGE_METADATA + requested_data_size;
	else {
		row_size = SIZE_OF_PAGE_METADATA + (SIZE_OF_BLOCK_METADATA +
					SIZE_OF_BLOCK_BODY(page_zone)) * INITIAL_NUM_OF_BLOCKS_PER_PAGE;
	}
	aligned_size = row_size + getpagesize() - row_size % getpagesize();
	new_page = (t_page*)mmap(0, aligned_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (MAP_FAILED == new_page)
		return (NULL);
	new_page_init(new_page, aligned_size);
	if (LARGE == page_zone) {
		new_page->occupied_size = requested_data_size;
		new_page->is_free = FALSE;
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
	printf("FT_MALLOC\n");

	t_page	*page;
	t_block *new_block;
	e_zone page_zone;

	if (!size)
		return (NULL);
	page_zone = get_page_zone_enum(size);
	page = g_zones_db[page_zone];
	if (LARGE != page_zone) {
		while (page && !page->is_free)
			page = page->next;
	}
	if ((LARGE == page_zone) || !page) {
		printf("FT_MALLOC: no free page found\n");
		if ((page = create_new_page(page_zone, size)) == NULL)
			return (NULL);
		add_new_page_to_db(page, page_zone);
		if (LARGE == page_zone)
			return ((void *)page + SIZE_OF_PAGE_METADATA);
	}
	if ((new_block = try_to_occupy_free_block(page, size)) == NULL)
			new_block = create_new_block(page, size, SIZE_OF_BLOCK_BODY(page_zone));

	void *possible_next_block_ptr       = (void *)new_block + SIZE_OF_BLOCK_METADATA + SIZE_OF_BLOCK_BODY(page_zone);
	void *valid_start_of_last_block_ptr = (void *)page + SIZE_OF_PAGE_METADATA + page->total_size -
										(SIZE_OF_BLOCK_METADATA + SIZE_OF_BLOCK_BODY(page_zone));
	if (possible_next_block_ptr >= valid_start_of_last_block_ptr)
	{
		printf("FT_MALLOC: Page is not free now\n");
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
	new_ptr = ft_malloc(size);
	pthread_mutex_unlock(&g_mutex_malloc);
	return (new_ptr);
}

t_page	*find_page_with_block(void *ptr, e_zone page_zone)
{
	t_page *page;
	t_block *block;
	t_block *block_to_find;

	if ((page = g_zones_db[page_zone]) == NULL)///do i need page_zone???
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

	page = g_zones_db[LARGE];
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

void	free_block(t_page *page, void *ptr, e_zone page_zone)
{
	printf("FREE_BLOCK\n");
	t_block *block_to_free;
	t_block *free_block;

	block_to_free = (t_block *)(ptr - SIZE_OF_BLOCK_METADATA);
	
	if (page->first_occupied_block = block_to_free) {
		printf("FREE_BLOCK: first_occupied_block is being freed\n");
		page->first_occupied_block = block_to_free->next;
	}
	if (block_to_free->prev)
		block_to_free->prev->next = block_to_free->next;
	if (block_to_free->next)
		block_to_free->next->prev = block_to_free->prev;
	if (!page->first_free_block) {
		printf("FREE_BLOCK: first_free_block is now added\n");
		page->first_free_block = block_to_free;
		block_to_free->prev = NULL;
	}
	else {
		printf("FREE_BLOCK: free_block is now added to the list\n");
		free_block = page->first_free_block;
		while (free_block->next)
			free_block = free_block->next;
		free_block->next = block_to_free;
		block_to_free->prev = free_block;
	}
	block_to_free->next = NULL;
	page->is_free = TRUE;
}

void	free_large_page(t_page *page_to_free)
{
	printf("FREE_LARGE_PAGE\n");

	if (g_zones_db[LARGE] == page_to_free)
		g_zones_db[LARGE] = page_to_free->next;
	if (page_to_free->prev)
		page_to_free->prev->next = page_to_free->next;
	if (page_to_free->next)
		page_to_free->next->prev = page_to_free->prev;
	munmap(page_to_free, (page_to_free->total_size + SIZE_OF_PAGE_METADATA));
}

void	*realloc_block(t_page *page, e_zone old_page_zone, void *old_mem_ptr, size_t new_size)
{
	printf("REALLOC_BLOCK\n");
	t_block *old_block;
	e_zone	new_page_zone;
	void   *new_mem_ptr;

	old_block = (t_block *)(old_mem_ptr - SIZE_OF_BLOCK_METADATA);
	printf("old_block size: %ld\n", old_block->occupied_size);
	printf("new size: %ld\n", new_size);
	new_page_zone = get_page_zone_enum(new_size);
	if ((new_page_zone == old_page_zone) &&
		(new_size <= old_block->total_size)) ///size is default block size REDO //old_block->occupied_size
	{
		printf("REALLOC_LARGE_PAGE: NEW SIZE REQUESTED IS LESS THEN PREVIOUS, RETURNING THE SAME PTR\n");
		//split the rest?
		old_block->occupied_size = new_size;
		return (old_mem_ptr);
	}
	if ((new_mem_ptr = ft_malloc(new_size)) == NULL)
		return NULL;
	memcpy(new_mem_ptr, old_mem_ptr, MIN(new_size, old_block->occupied_size));	//will copy with extra bytes, redo
	free_block(page, old_mem_ptr, old_page_zone);
	return (new_mem_ptr);
}

void	*realloc_large_page(void *old_mem_ptr, size_t new_size)
{
	printf("REALLOC_LARGE_PAGE\n");

	t_page	*page;
	t_page	*old_page;
	void	*new_mem_ptr;
	e_zone	page_zone_for_new_size;
	int		aligned_new_size;
	
	old_page = (t_page *)(old_mem_ptr - SIZE_OF_PAGE_METADATA);
	page_zone_for_new_size = get_page_zone_enum(new_size);

	if (LARGE == page_zone_for_new_size) { //is there is a sense in substracting page header from total size?
		aligned_new_size = (SIZE_OF_PAGE_METADATA + new_size) + getpagesize() -
							(SIZE_OF_PAGE_METADATA + new_size) % getpagesize();
		if (aligned_new_size == (old_page->total_size + SIZE_OF_PAGE_METADATA)) {
			printf("REALLOC_LARGE_PAGE: NEW SIZE REQUESTED WILL GIVE SAME SIZE TO MALLOC, RETURNING THE SAME PTR\n");
			old_page->occupied_size = new_size;
			return old_mem_ptr;
		}
	}
	printf("REALLOC_LARGE_PAGE: g_zones_db[LARGE] before new_page creation is: %p\n", g_zones_db[LARGE]);
	if ((new_mem_ptr = ft_malloc(new_size)) == NULL)
		return NULL;
	printf("REALLOC_LARGE_PAGE: g_zones_db[LARGE] after new_page creation is: %p\n", g_zones_db[LARGE]);
	memcpy(new_mem_ptr, old_mem_ptr, MIN(new_size, old_page->occupied_size));
	if (LARGE == page_zone_for_new_size)
		printf("REALLOC_LARGE_PAGE: new_page address is: %p\n", new_mem_ptr - SIZE_OF_PAGE_METADATA);
	else
		printf("REALLOC_LARGE_PAGE: new_page address is: %p\n", new_mem_ptr - SIZE_OF_BLOCK_METADATA);
	printf("REALLOC_LARGE_PAGE: old_page address is: %p\n", (void *)old_page);	
	free_large_page(old_page);
	return (new_mem_ptr);
}

void		ft_free(void *ptr)
{
	printf("FT_FREE\n");
	e_zone 	page_zone;
	t_page	*page;

	if (!ptr)
		return;
	page_zone = TINY;
	if ((page = find_page_with_ptr(ptr, &page_zone)) == NULL)
		return;
	if (LARGE == page_zone)
		return free_large_page(page);
	else
		return free_block(page, ptr, page_zone);
}

void	s_free(void *ptr)
{
	printf("S_FREE\n");
	pthread_mutex_lock(&g_mutex_malloc);
	ft_free(ptr);
	pthread_mutex_unlock(&g_mutex_malloc);
}

void	*ft_realloc(void *ptr, size_t size)
{
	printf("FT_REALLOC\n");
	e_zone 	page_zone;
	t_page	*page;

	if (!ptr)
		return (s_malloc(size));
	printf("size: %ld\n", size);
	if (!size)
	{
		s_free(ptr);
		return NULL;
	}
	page_zone = TINY;
	if ((page = find_page_with_ptr(ptr, &page_zone)) == NULL) {
		printf("FT_REALLOC: PTR IS NOT VALID\n");
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
	new_ptr = ft_realloc(ptr, size);
	pthread_mutex_unlock(&g_mutex_malloc);
	return (new_ptr);
}