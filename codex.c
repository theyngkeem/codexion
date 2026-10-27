/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 09:21:16 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/26 23:11:19 by yaarab           ###   ########.fr       */
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

	data->start_time = now_ms();
	i = 0;
	while (i < data->number_of_coders)
	{
		data->coder[i].last_compile = data->start_time;
		i++;
	}
}

static int	run_simulation(t_data *data)
{
	set_start_time(data);
	if (!start_threads(data))
		return (0);
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
