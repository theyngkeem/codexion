/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 01:55:31 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/28 05:48:43 by yaarab           ###   ########.fr       */
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

static int	reserve_dongles(t_coder *coder)
{
	t_data	*data;
	t_dongle	*first;
	t_dongle	*second;
	int		a;
	int		b;
	int		success;
	long	time;

	data = coder->data;
	a = coder->left_dongel;
	b = coder->right_dongel;
	if (a == b)
		return (0);
	if (a > b)
	{
		a = coder->right_dongel;
		b = coder->left_dongel;
	}
	first = &data->dongle[a];
	second = &data->dongle[b];
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	time = now_ms();
	success = (!first->in_use && !second->in_use
			&& time >= first->available_at
			&& time >= second->available_at);
	if (success)
	{
		first->in_use = 1;
		second->in_use = 1;
	}
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	return (success);
}

static int	has_priority_conflict(t_data *data, t_request req)
{
	t_request	other;
	t_coder		*a;
	t_coder		*b;
	int			i;

	a = &data->coder[req.coder_id - 1];
	i = 0;
	while (i < data->waiting->size)
	{
		other = data->waiting->arr[i];
		b = &data->coder[other.coder_id - 1];
		if (other.coder_id != req.coder_id
			&& request_before(data, other, req)
			&& (a->left_dongel == b->left_dongel
				|| a->left_dongel == b->right_dongel
				|| a->right_dongel == b->left_dongel
				|| a->right_dongel == b->right_dongel))
			return (1);
		i++;
	}
	return (0);
}

static int	try_grant_requests(t_data *data)
{
	t_request	req;
	t_coder		*coder;
	int			i;
	int			granted;

	i = 0;
	granted = 0;
	while (i < data->waiting->size)
	{
		req = data->waiting->arr[i];
		coder = &data->coder[req.coder_id - 1];
		if (!has_priority_conflict(data, req)
			&& reserve_dongles(coder))
		{
			heap_remove_at(data->waiting, i, &req);
			coder->request_pending = 0;
			coder->dongles_granted = 1;
			granted++;
			i = 0;
		}
		else
			i++;
	}
	if (granted)
		pthread_cond_broadcast(&data->sch_cond);
	return (granted);
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
		try_grant_requests(data);
	}
	pthread_mutex_unlock(&data->sch_mutex);
	return (success);
}

void	release_dongles(t_coder *coder)
{
	t_data	*data;
	int		a;
	int		b;
	long	cooldown;

	data = coder->data;
	a = coder->left_dongel;
	b = coder->right_dongel;
	if (a > b)
	{
		a = coder->right_dongel;
		b = coder->left_dongel;
	}
	if (a == b)
		return ;
	pthread_mutex_lock(&data->sch_mutex);
	pthread_mutex_lock(&data->dongle[a].mutex);
	pthread_mutex_lock(&data->dongle[b].mutex);
	cooldown = now_ms() + data->dongle_cooldown;
	data->dongle[a].in_use = 0;
	data->dongle[b].in_use = 0;
	data->dongle[a].available_at = cooldown;
	data->dongle[b].available_at = cooldown;
	pthread_mutex_unlock(&data->dongle[b].mutex);
	pthread_mutex_unlock(&data->dongle[a].mutex);
	coder->dongles_granted = 0;
	pthread_cond_broadcast(&data->sch_cond);
	pthread_mutex_unlock(&data->sch_mutex);
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
		try_grant_requests(data);
		granted = coder->dongles_granted;
		pthread_mutex_unlock(&data->sch_mutex);
		if (granted)
			return (1);
		usleep(500);
	}
	return (0);
}
