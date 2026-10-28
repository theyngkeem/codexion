/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 09:21:16 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/28 05:37:01 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

static int	allocate_data(t_data *data)
{
	data->coder = malloc(sizeof(t_coder) * data->number_of_coders);
	if (!data->coder)
		return (0);
	data->dongle = malloc(sizeof(t_dongle) * data->number_of_coders);
	if (!data->dongle)
	{
		free(data->coder);
		return (0);
	}
	return (1);
}

static int	init_data(t_data *data)
{
	if (!init_dongles(data))
	{
		free(data->dongle);
		free(data->coder);
		return (0);
	}
	init_coder(data);
	if (!init_scheduler(data))
	{
		cleanup_simulation(data);
		return (0);
	}
	if (!init_shared_state(data))
	{
		cleanup_scheduler(data);
		cleanup_simulation(data);
		return (0);
	}
	return (1);
}

static void	set_start_time(t_data *data)
{
	int	i;

	pthread_mutex_lock(&data->state_mutex);
	data->start_time = now_ms();
	i = 0;
	while (i < data->number_of_coders)
	{
		data->coder[i].last_compile = data->start_time;
		i++;
	}
	pthread_mutex_unlock(&data->state_mutex);
}

static int	run_simulation(t_data *data)
{
	pthread_t	monitor;

	if (pthread_create(&monitor, NULL, monitor_routine, data) != 0)
		return (0);
	if (!start_threads(data))
	{
		pthread_mutex_lock(&data->state_mutex);
		data->stop = 1;
		pthread_mutex_unlock(&data->state_mutex);
		pthread_join(monitor, NULL);
		return (0);
	}
	set_start_time(data);
	pthread_join(monitor, NULL);
	join_threads(data, data->number_of_coders);
	return (1);
}

int	main(int ac, char **av)
{
	t_data	data;
	int		success;

	memset(&data, 0, sizeof(t_data));
	if (!parse_args(ac, av, &data))
		return (1);
	if (!allocate_data(&data))
		return (1);
	if (!init_data(&data))
		return (1);
	success = run_simulation(&data);
	cleanup_shared_state(&data);
	cleanup_scheduler(&data);
	cleanup_simulation(&data);
	return (!success);
}
