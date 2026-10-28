/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 09:21:16 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/28 05:37:01 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

t_heap	*creat_heap(int capacity, t_scheduler scheduler)
{
	t_heap	*heap;

	if (capacity <= 0)
		return (NULL);
	heap = malloc(sizeof(t_heap));
	if (!heap)
		return (NULL);
	heap->size = 0;
	heap->capacity = capacity;
	heap->scheduler = scheduler;
	heap->arr = malloc(capacity * sizeof(t_request));
	if (!heap->arr)
	{
		free(heap);
		return (NULL);
	}
	return (heap);
}

int	higher_priority(t_heap *heap, t_request a, t_request b)
{
	if (heap->scheduler == EDF && a.deadline != b.deadline)
		return (a.deadline < b.deadline);
	if (a.sequence != b.sequence)
		return (a.sequence < b.sequence);
	return (a.coder_id < b.coder_id);
}

int	push_heap(t_heap *heap, t_request req)
{
	int			i;
	int			parent;
	t_request	tmp;

	if (!heap || heap->size >= heap->capacity)
		return (0);
	i = heap->size;
	heap->arr[i] = req;
	heap->size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!higher_priority(heap, heap->arr[i], heap->arr[parent]))
			break ;
		tmp = heap->arr[parent];
		heap->arr[parent] = heap->arr[i];
		heap->arr[i] = tmp;
		i = parent;
	}
	return (1);
}

void	free_heap(t_heap *heap)
{
	if (!heap)
		return ;
	free(heap->arr);
	free(heap);
}
