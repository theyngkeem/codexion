/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 09:21:16 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/28 05:37:01 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

static int	reserve_pair(t_dongle *first, t_dongle *second,
		int reserve, long time)
{
	int	success;

	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
	success = (!first->in_use && !second->in_use
			&& time >= first->available_at
			&& time >= second->available_at);
	if (success && reserve)
	{
		first->in_use = 1;
		second->in_use = 1;
	}
	pthread_mutex_unlock(&second->mutex);
	pthread_mutex_unlock(&first->mutex);
	return (success);
}

int	reserve_dongles(t_coder *coder, int reserve, long time)
{
	t_data		*data;
	t_dongle	*first;
	t_dongle	*second;
	int			a;
	int			b;

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
	return (reserve_pair(first, second, reserve, time));
}

static void	release_pair(t_data *data, int a, int b, t_coder *coder)
{
	long	cooldown;

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

void	release_dongles(t_coder *coder)
{
	t_data	*data;
	int		a;
	int		b;

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
	release_pair(data, a, b, coder);
}
