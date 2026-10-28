
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

static int	higher_priority(t_heap *heap, t_request a, t_request b)
{
	if (heap->scheduler == EDF && a.deadline != b.deadline)
		return (a.deadline < b.deadline);
	if (a.sequence != b.sequence)
		return (a.sequence < b.sequence);
	return (a.coder_id < b.coder_id);
}

int	heap_remove_at(t_heap *heap, int index, t_request *removed)
{
	int			i;
	int			parent;
	int			child;
	t_request	tmp;

	if (!heap || !removed || index < 0 || index >= heap->size)
		return (0);
	*removed = heap->arr[index];
	heap->size--;
	if (index == heap->size)
		return (1);
	heap->arr[index] = heap->arr[heap->size];
	i = index;
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
	return (1);
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

int	heap_pop(t_heap *heap, t_request *res)
{
	int			i;
	int			left;
	int			right;
	int			small;
	t_request	tmp;

	if (!heap || !res || heap->size == 0)
		return (0);
	*res = heap->arr[0];
	heap->size--;
	if (heap->size == 0)
		return (1);
	heap->arr[0] = heap->arr[heap->size];
	i = 0;
	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		small = i;
		if (left < heap->size && higher_priority(heap,
				heap->arr[left], heap->arr[small]))
			small = left;
		if (right < heap->size && higher_priority(heap,
				heap->arr[right], heap->arr[small]))
			small = right;
		if (small == i)
			break ;
		tmp = heap->arr[i];
		heap->arr[i] = heap->arr[small];
		heap->arr[small] = tmp;
		i = small;
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
