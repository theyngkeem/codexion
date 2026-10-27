/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 23:19:12 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/27 08:58:08 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

static int	check_coders(t_data *data)
{
	int		i;
	int		finished;
	long	current;

	i = 0;
	finished = 0;
	current = now_ms();
	while (i < data->number_of_coders)
	{
		if (current - data->coder[i].last_compile
			>= data->time_to_burnout)
			return (data->coder[i].id);
		if (data->coder[i].complie_c >= data->number_of_compiles_req)
			finished++;
		i++;
	}
	if (finished == data->number_of_coders)
		return (-1);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_data	*data;
	int		result;

	data = (t_data *)arg;
	while (!simulation_stopped(data))
	{
		pthread_mutex_lock(&data->state_mutex);
		result = check_coders(data);
		if (result != 0)
			data->stop = 1;
		pthread_mutex_unlock(&data->state_mutex);
		if (result > 0)
		{
			pthread_mutex_lock(&data->log_mutex);
			printf("%ld %d burned out\n",
				now_ms() - data->start_time, result);
			pthread_mutex_unlock(&data->log_mutex);
		}
		if (result != 0)
			break ;
		usleep(500);
	}
	return (NULL);
}
