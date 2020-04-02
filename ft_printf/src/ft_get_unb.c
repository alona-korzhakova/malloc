/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_get_unb.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akorzhak <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/11 16:45:31 by akorzhak          #+#    #+#             */
/*   Updated: 2018/01/11 16:45:33 by akorzhak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libftprintf.h"

static int			size_of_arr(uintmax_t value, int base)
{
	int				len;

	len = 1;
	while (value >= (uintmax_t)base)
	{
		value /= base;
		len++;
	}
	return (len);
}

static char			*itoa_base(uintmax_t v, int base, char **f)
{
	char			*arr;
	int				len;
	char			a;

	if (**f == 'x' || **f == 'p')
		a = 'a';
	else
		a = 'A';
	len = size_of_arr(v, base);
	arr = ft_strnew(len--);
	while (v > 0)
	{
		if ((v % base) < 10)
			arr[len--] = (char)(v % base + '0');
		else
			arr[len--] = (char)(v % base - 10 + a);
		v /= base;
	}
	return (arr);
}

static char			*convert_to_char(uintmax_t nb, char **f)
{
	char *str;
	if (!nb) 
	{
		if (**f == 'x' || **f == 'X')
		{
			str = ft_strnew(2);
			str[1] = '0';
		}
		else
			str = ft_strnew(1);
		str[0] = '0';
		return (str);
	}
	if (**f == 'o' || **f == 'O')
		return (itoa_base(nb, 8, f));
	else if (**f == 'u' || **f == 'U')
		return (itoa_base(nb, 10, f));
	else if (**f == 'X')
		return (itoa_base(nb, 16, f));
	else if (**f == 'x')
		return (itoa_base(nb, 16, f));
	else
		return (itoa_base(nb, 16, f));
}

char				*ft_get_unb(t_flags *ptr, char **f, va_list arg)
{
	uintmax_t	nb;

	if (ft_strchr("OU", **f))
		nb = va_arg(arg, unsigned long);
	else if (ptr->h)
		nb = (unsigned short)(va_arg(arg, uintmax_t));
	else if (ptr->hh)
		nb = (unsigned char)(va_arg(arg, unsigned int));
	else if (ptr->l)
		nb = va_arg(arg, unsigned long);
	else if (ptr->ll)
		nb = va_arg(arg, unsigned long long);
	else if (ptr->z)
		nb = va_arg(arg, size_t);
	else if (ptr->j)
		nb = va_arg(arg, uintmax_t);
	else if (ft_strchr("ouxX", **f))
		nb = va_arg(arg, unsigned int);
	else if (**f == 'p')
		nb = va_arg(arg, uintmax_t);	
	return (convert_to_char(nb, f));
}
