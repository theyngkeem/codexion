/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_pop.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 09:21:16 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/28 05:37:01 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

static int	heap_pop_child(t_heap *heap, int i)
{
	int	left;
	int	right;
	int	small;

	left = 2 * i + 1;
	right = 2 * i + 2;
	small = i;
	if (left < heap->size && higher_priority(heap,
			heap->arr[left], heap->arr[small]))
		small = left;
	if (right < heap->size && higher_priority(heap,
			heap->arr[right], heap->arr[small]))
		small = right;
	return (small);
}

static void	heap_pop_down(t_heap *heap)
{
	int			i;
	int			small;
	t_request	tmp;

	i = 0;
	while (1)
	{
		small = heap_pop_child(heap, i);
		if (small == i)
			break ;
		tmp = heap->arr[i];
		heap->arr[i] = heap->arr[small];
		heap->arr[small] = tmp;
		i = small;
	}
}

int	heap_pop(t_heap *heap, t_request *res)
{
	if (!heap || !res || heap->size == 0)
		return (0);
	*res = heap->arr[0];
	heap->size--;
	if (heap->size == 0)
		return (1);
	heap->arr[0] = heap->arr[heap->size];
	heap_pop_down(heap);
	return (1);
}
