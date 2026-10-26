/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 01:55:31 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/26 21:29:48 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

static int	dongle_available(t_dongle *dongle, long current_time)
{
	return (!dongle->in_use
		&& current_time >= dongle->available_at);
}

static int	can_acquire(t_coder *coder, long current_time)
{
	t_data		*data;
	t_dongle	*left;
	t_dongle	*right;

	data = coder->data;
	if (coder->left_dongel == coder->right_dongel)
		return (0);
	left = &data->dongle[coder->left_dongel];
	right = &data->dongle[coder->right_dongel];
	if (!dongle_available(left, current_time))
		return (0);
	if (!dongle_available(right, current_time))
		return (0);
	return (1);
}

static int	reserve_dongles(t_coder *coder)
{
	t_data		*data;
	int			first;
	int			second;
	int			success;

	data = coder->data;
	first = coder->left_dongel;
	second = coder->right_dongel;
	if (first == second)
		return (0);
	if (first > second)
	{
		first = coder->right_dongel;
		second = coder->left_dongel;
	}
	pthread_mutex_lock(&data->dongle[first].mutex);
	pthread_mutex_lock(&data->dongle[second].mutex);
	success = can_acquire(coder, now_ms());
	if (success)
	{
		data->dongle[first].in_use = 1;
		data->dongle[second].in_use = 1;
	}
	pthread_mutex_unlock(&data->dongle[second].mutex);
	pthread_mutex_unlock(&data->dongle[first].mutex);
	return (success);
}

static int	try_grant_requests(t_data *data)
{
	int	g;

	g = 0;
	return (g);
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
	req.deadline = coder->last_compile + data->time_to_burnout;
	req.sequence = data->request_seq;
	success = push_heap(data->waiting, req);
	if (success)
	{
		data->request_seq++;
		coder->request_pending = 1;
		coder->dongles_granted = 0;
		pthread_cond_signal(&data->sch_cond);
	}
	pthread_mutex_unlock(&data->sch_mutex);
	return (success);
}
