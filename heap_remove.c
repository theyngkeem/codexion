/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_remove.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 09:21:16 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/28 05:37:01 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

static int	heap_remove_up(t_heap *heap, int i)
{
	int			parent;
	t_request	tmp;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!higher_priority(heap, heap->arr[i], heap->arr[parent]))
			break ;
		tmp = heap->arr[i];
		heap->arr[i] = heap->arr[parent];
		heap->arr[parent] = tmp;
		i = parent;
	}
	return (i);
}

static void	heap_remove_down(t_heap *heap, int i)
{
	int			child;
	t_request	tmp;

	while (1)
	{
		child = 2 * i + 1;
		if (child >= heap->size)
			break ;
		if (child + 1 < heap->size
			&& higher_priority(heap, heap->arr[child + 1],
				heap->arr[child]))
			child++;
		if (!higher_priority(heap, heap->arr[child], heap->arr[i]))
			break ;
		tmp = heap->arr[i];
		heap->arr[i] = heap->arr[child];
		heap->arr[child] = tmp;
		i = child;
	}
}

int	heap_remove_at(t_heap *heap, int index, t_request *removed)
{
	int	i;

	if (!heap || !removed || index < 0 || index >= heap->size)
		return (0);
	*removed = heap->arr[index];
	heap->size--;
	if (index == heap->size)
		return (1);
	heap->arr[index] = heap->arr[heap->size];
	i = index;
	i = heap_remove_up(heap, i);
	heap_remove_down(heap, i);
	return (1);
}
