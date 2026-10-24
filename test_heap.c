
#include "codex.h"

int	main(void)
{
	t_heap		*heap;
	t_request	res;

	heap = creat_heap(5, FIFO);
	if (!heap)
		return (1);
	push_heap(heap, (t_request){1, 900, 1});
	push_heap(heap, (t_request){2, 600, 2});
	push_heap(heap, (t_request){3, 700, 3});
	push_heap(heap, (t_request){4, 400, 4});
	push_heap(heap, (t_request){5, 800, 5});
	while (heap_pop(heap, &res))
		printf("Coder %d: deadline %ld\n",
			res.coder_id, res.deadline);
	free_heap(heap);
	return (0);
}
