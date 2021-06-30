/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_vector.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ehakam <ehakam@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/01 15:30:30 by ehakam            #+#    #+#             */
/*   Updated: 2021/05/23 16:12:37 by ehakam           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_vector.h"

void	ft_exit(char *msg, int code)
{
	if (msg)
		perror(msg);
	exit(code);
}

t_vector	*new_vector(void)
{
	return (new_vector_s(10));
}

t_vector	*new_vector_s(t_size init_len)
{
	t_vector	*new_vector;

	new_vector = (t_vector *) malloc(sizeof(t_vector));
	if (!new_vector)
		ft_exit(ERR_MALLOC, 1);
	new_vector->data = (void **) malloc(sizeof(void *) * init_len);
	if (!new_vector->data)
		ft_exit(ERR_MALLOC, 1);
	new_vector->capacity = init_len;
	new_vector->size = 0;
	return (new_vector);
}

t_vector	*new_vector_from(t_vector *other)
{
	int			i;
	t_size		length;
	t_vector	*new_vector;

	if (other == NULL || is_empty(other))
		ft_exit("Error\nVector is NULL/Empty", 1);
	length = other->size + other->size / 2;
	new_vector = new_vector_s(length);
	i = -1;
	while (++i < other->size)
		new_vector->data[new_vector->size++] = other->data[i];
	return (new_vector);
}

void	*at(t_vector *this, size_t pos)
{
	if (this == NULL)
		ft_exit("Error\nAccessing items of a NULL Vector!", 1);
	if (pos >= this->size)
		ft_exit("Error\nIndex Out Of Bounds!", 1);
	return (this->data[pos]);
}
