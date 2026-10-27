/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   state.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 02:26:36 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/26 23:11:12 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

int	simulation_stopped(t_data *data)
{
	int	stopped;

	pthread_mutex_lock(&data->state_mutex);
	stopped = data->stop;
	pthread_mutex_unlock(&data->state_mutex);
	return (stopped);
}

int	wait_ms(t_data *data, long duration)
{
	long	start;

	start = now_ms();
	while (!simulation_stopped(data))
	{
		if (now_ms() - start >= duration)
			return (1);
		usleep(500);
	}
	return (0);
}

void	log_state(t_coder *coder, char *message)
{
	t_data	*data;

	data = coder->data;
	pthread_mutex_lock(&data->log_mutex);
	if (!simulation_stopped(data))
	{
		printf("%ld %d %s\n",
			now_ms() - data->start_time,
			coder->id, message);
	}
	pthread_mutex_unlock(&data->log_mutex);
}

long	now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}
