/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_memory.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorzhak <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/01 12:57:19 by akorzhak          #+#    #+#             */
/*   Updated: 2020/03/01 12:57:20 by akorzhak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

void	print_address(long long addr)
{
	int len;

	len = sizeof(long long) * NUM_OF_PRINTED_SYMBOLS_PER_BYTE + 1;
	char arr[len];	
	arr[len--] = 0;
	while (addr > 0)
	{
		char reminder = (char)(addr % 16);
		arr[len--] = (reminder < 10) ? (reminder + '0') : (reminder -10 + 'a');
		addr /= 16;
	}
	ft_putstr(&arr[++len]);
}

void	ft_hexdump(char *ptr, size_t size)
{
	size_t byte_ix;

	byte_ix = 0;
	while (byte_ix < size) {
		if (!(byte_ix % (NUM_OF_BYTES_PER_CHUNK * NUM_OF_CHUNKS_PER_LINE)))
		{
			ft_putchar('\n');
			print_address((long long)(&ptr[byte_ix]));
			ft_putstr(": ");
		}	
		else if (!(byte_ix % NUM_OF_BYTES_PER_CHUNK))
			ft_putchar(' ');
		char msb_num = ptr[byte_ix] / 16;	
		char lsb_num = ptr[byte_ix] % 16;
		char msb_symb = (msb_num < 10) ? (msb_num + '0') : (msb_num - 10 + 'a');
		char lsb_symb = (lsb_num < 10) ? (lsb_num + '0') : (lsb_num - 10 + 'a');
		ft_putchar(msb_symb);
		ft_putchar(lsb_symb);
		byte_ix++;
	}
	if (byte_ix % (NUM_OF_BYTES_PER_CHUNK * NUM_OF_CHUNKS_PER_LINE))
		ft_putchar('\n');
}

void show_info_on_page(t_page *page, int num_of_page, int show_hexdump)
{
	ft_putstr("\n__________________Allocation: ");
	print_address((long long)((void *)page + SIZE_OF_PAGE_METADATA));
	ft_putstr(", Page #");
	ft_putnbr(num_of_page);
	ft_putendl("_________________\n");
	ft_putstr("1. Allocated size:             ");
	ft_putnbr(page->allocated_size);
	ft_putchar('\n');
	ft_putstr("2. Allocated + Reserved size:  ");
	ft_putnbr(page->total_size);
	ft_putchar('\n');
	if (show_hexdump)
	{ 
		ft_putendl("3. Hexdump:");	
		ft_hexdump((void *)page + SIZE_OF_PAGE_METADATA, page->allocated_size);
	}
}

size_t show_info_on_large_zone(int show_hexdump)
{
	t_page *page;
	size_t num_of_page = 0;
	size_t bytes = 0;
	
	if ((page = g_malloc_db[LARGE]) == NULL) {
		ft_putendl("Zone is not allocated");
		return (bytes);
	}
	while (page)
	{
		num_of_page++;
		show_info_on_page(page, num_of_page, show_hexdump);
		bytes += page->allocated_size;
		page = page->next;
	}
	return (bytes);
}

void show_info_on_block(t_block *block, int num_of_page, int show_hexdump)
{
	ft_putstr("\n__________________Allocation: ");
	print_address((long long)((void *)block + SIZE_OF_BLOCK_METADATA));
	ft_putstr(", Page #");
	ft_putnbr(num_of_page);
	ft_putendl("_________________\n");
	ft_putstr("1. Allocated size:             ");
	ft_putnbr(block->allocated_size);
	ft_putchar('\n');
	ft_putstr("2. Allocated + Reserved size:  ");
	ft_putnbr(block->total_size);
	ft_putchar('\n');
	if (show_hexdump)
	{ 
		ft_putendl("3. Hexdump:");	
		ft_hexdump((void *)block + SIZE_OF_BLOCK_METADATA, block->allocated_size);
	}
}

size_t show_info_on_tiny_small_zones(e_zone page_zone, int show_hexdump)
{
	t_page *page;
	t_num_alloc num_alloc = {0, 0};
	t_block *block;
	size_t bytes = 0;
	
	if ((page = g_malloc_db[page_zone]) == NULL) {
		ft_putendl("Zone is not allocated");
		return (bytes);
	}
	while (page)
	{
		num_alloc.num_of_page++;
		num_alloc.num_of_block = 0;
		if ((block = page->first_occupied_block) == NULL) {
			ft_putstr("Page #");
			ft_putnbr(num_alloc.num_of_page);
			ft_putendl(" is empty");
			continue;
		}
		while (block) {
			num_alloc.num_of_block++;
			show_info_on_block(block, num_alloc.num_of_page, show_hexdump);
			bytes += block->allocated_size;
			block = block->next;
		}
		page = page->next;
	}
	return (bytes);
}

void	show_alloc_mem(int show_hexdump)
{
	size_t bytes_tiny;
	size_t bytes_small;
	size_t bytes_large;	

	pthread_mutex_lock(&g_mutex_malloc);
	ft_putendl("================ Allocated memory dump ================");
	ft_putendl("\n****************** Info on zone: TINY *******************\n");
	bytes_tiny = show_info_on_tiny_small_zones(TINY, show_hexdump);
	ft_putendl("\n****************** Info on zone: SMALL ******************\n");
	bytes_small = show_info_on_tiny_small_zones(SMALL, show_hexdump);
	ft_putendl("\n****************** Info on zone: LARGE ******************\n");
	bytes_large = show_info_on_large_zone(show_hexdump);
	ft_putstr("\nNum of allocated bytes in TINY:   ");
	ft_putnbr(bytes_tiny);
	ft_putchar('\n');	
	ft_putstr("Num of allocated bytes in SMALL:   ");
	ft_putnbr(bytes_small);
	ft_putchar('\n');
	ft_putstr("Num of allocated bytes in LARGE:   ");
	ft_putnbr(bytes_large);
	ft_putchar('\n');
	pthread_mutex_unlock(&g_mutex_malloc);
}