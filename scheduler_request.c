/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_request.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 09:21:16 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/28 05:37:01 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

static int	queue_request(t_coder *coder, t_request req)
{
	t_data	*data;
	int		success;

	data = coder->data;
	success = push_heap(data->waiting, req);
	if (success)
	{
		data->request_seq++;
		coder->request_pending = 1;
		coder->dongles_granted = 0;
		pthread_cond_signal(&data->sch_cond);
	}
	return (success);
}

int	submit_request(t_coder *coder)
{
	t_data		*data;
	t_request	req;
	int			success;

	data = coder->data;
	pthread_mutex_lock(&data->sch_mutex);
	if (coder->request_pending || coder->dongles_granted)
	{
		pthread_mutex_unlock(&data->sch_mutex);
		return (0);
	}
	req.coder_id = coder->id;
	pthread_mutex_lock(&data->state_mutex);
	req.deadline = coder->last_compile + data->time_to_burnout;
	pthread_mutex_unlock(&data->state_mutex);
	req.sequence = data->request_seq;
	success = queue_request(coder, req);
	pthread_mutex_unlock(&data->sch_mutex);
	return (success);
}

static void	release_pending_grant(t_coder *coder)
{
	t_data	*data;
	int		granted;

	data = coder->data;
	pthread_mutex_lock(&data->sch_mutex);
	granted = coder->dongles_granted;
	pthread_mutex_unlock(&data->sch_mutex);
	if (granted)
		release_dongles(coder);
}

int	wait_for_dongles(t_coder *coder)
{
	t_data	*data;
	int		granted;

	data = coder->data;
	if (!submit_request(coder))
		return (0);
	while (!simulation_stopped(data))
	{
		pthread_mutex_lock(&data->sch_mutex);
		if (!simulation_stopped(data))
			try_grant_requests(data);
		granted = coder->dongles_granted;
		pthread_mutex_unlock(&data->sch_mutex);
		if (granted)
			return (1);
		usleep(500);
	}
	release_pending_grant(coder);
	return (0);
}
