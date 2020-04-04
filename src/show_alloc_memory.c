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
	int start_ix;

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
	ft_putstr(": ");
}

void	ft_hexdump(char *ptr, size_t size)
{
	int byte_ix;

	byte_ix = 0;
	while (byte_ix < size) {
		if (!(byte_ix % (NUM_OF_BYTES_PER_CHUNK * NUM_OF_CHUNKS_PER_LINE)))
		{
			ft_putchar('\n');
			print_address((long long)(&ptr[byte_ix]));
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

char *page_zone_to_string(e_zone page_zone)
{
	if (TINY == page_zone)
		return ("TINY");
	if (SMALL == page_zone)
		return ("SMALL");
	if (LARGE == page_zone)
		return ("LARGE");
}

void	show_info_on_tiny_small_pages(e_zone page_zone)
{
	t_page *page;
	t_block *block;
	int num_of_block;
	int num_of_page;

	ft_putchar('\n');
	ft_putstr("**************** Info on zone: ");
	ft_putstr(page_zone_to_string(page_zone));
	ft_putendl(" ****************");
	ft_putchar('\n');
	if ((page = g_malloc_db[page_zone]) == NULL) {
		ft_putendl("Zone is not allocated");
		return;
	}
	num_of_page = 0;
	while (page)
	{
		num_of_page++;
		num_of_block = 0;
		if ((block = page->first_occupied_block) == NULL) {
			ft_putstr("Page #");
			ft_putnbr(num_of_page);
			ft_putendl(" is empty");
			continue;
		}
		while (block) {
			num_of_block++;
			ft_putstr("_______________________Block #");
			ft_putnbr(num_of_block);
			ft_putendl("______________________");

			ft_putstr("1. Reserved size:    ");
			ft_putnbr(block->reserved_size);
			ft_putchar('\n');

			ft_putstr("2. Allocated size:   ");
			ft_putnbr(block->allocated_size);
			ft_putchar('\n');

			ft_putendl("3. Hexdump:        ");	
			ft_hexdump((void *)block + SIZE_OF_BLOCK_METADATA, block->allocated_size);
			block = block->next;
			ft_putchar('\n');
		}
		page = page->next;
	}
}

void	show_alloc_mem(void)
{
	ft_putendl("========== Allocated memory dump ==========");
//	size_t allocated_memory_size;

//	allocated_memory_size = 0;
	pthread_mutex_lock(&g_mutex_malloc);
	//allocated_memory_size += 
	show_info_on_tiny_small_pages(TINY);
	show_info_on_tiny_small_pages(SMALL);
	//allocated_memory_size +=
//	show_info_on_large_pages();
	//allocated_memory_size += show_info_on_large_pages();
	pthread_mutex_unlock(&g_mutex_malloc);
}