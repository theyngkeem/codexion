/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 01:55:31 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/28 06:27:06 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

static int	request_before(t_data *data, t_request a, t_request b)
{
	if (data->scheduler == EDF && a.deadline != b.deadline)
		return (a.deadline < b.deadline);
	if (a.sequence != b.sequence)
		return (a.sequence < b.sequence);
	return (a.coder_id < b.coder_id);
}

static int	find_ready_request(t_data *data)
{
	int		i;
	int		best;
	long	time;

	i = 0;
	best = -1;
	time = now_ms();
	while (i < data->waiting->size)
	{
		if ((best < 0 || request_before(data, data->waiting->arr[i],
					data->waiting->arr[best]))
			&& reserve_dongles(&data->coder[
					data->waiting->arr[i].coder_id - 1], 0, time))
			best = i;
		i++;
	}
	return (best);
}

int	try_grant_requests(t_data *data)
{
	t_request	req;
	t_coder		*coder;
	int			i;
	int			granted;

	granted = 0;
	i = find_ready_request(data);
	while (i >= 0)
	{
		req = data->waiting->arr[i];
		coder = &data->coder[req.coder_id - 1];
		if (!reserve_dongles(coder, 1, now_ms()))
			break ;
		heap_remove_at(data->waiting, i, &req);
		coder->request_pending = 0;
		coder->dongles_granted = 1;
		granted++;
		i = find_ready_request(data);
	}
	if (granted)
		pthread_cond_broadcast(&data->sch_cond);
	return (granted);
}
