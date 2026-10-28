/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threades.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 22:10:25 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/26 23:16:05 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

void	join_threads(t_data *data, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_join(data->coder[i].thread, NULL);
		i++;
	}
}

static int	begin_compile(t_coder *coder)
{
	t_data	*data;
	long	time;
	int		ready;

	data = coder->data;
	pthread_mutex_lock(&data->log_mutex);
	pthread_mutex_lock(&data->state_mutex);
	time = now_ms();
	ready = (!data->stop && time - coder->last_compile
			< data->time_to_burnout);
	if (ready)
		coder->last_compile = time;
	pthread_mutex_unlock(&data->state_mutex);
	if (ready)
	{
		printf("%ld %d has taken a dongle\n", time - data->start_time,
			coder->id);
		printf("%ld %d has taken a dongle\n", time - data->start_time,
			coder->id);
		printf("%ld %d is compiling\n", time - data->start_time, coder->id);
	}
	pthread_mutex_unlock(&data->log_mutex);
	return (ready);
}

static int	compile_once(t_coder *coder)
{
	t_data	*data;

	data = coder->data;
	if (!wait_for_dongles(coder))
		return (0);
	if (!begin_compile(coder) || !wait_ms(data, data->time_to_compile))
	{
		release_dongles(coder);
		return (0);
	}
	count_compile(coder);
	release_dongles(coder);
	return (coder->complie_c < data->number_of_compiles_req);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_data	*data;

	coder = (t_coder *)arg;
	data = coder->data;
	if (!wait_for_start(data) || data->number_of_compiles_req == 0)
		return (NULL);
	while (!simulation_stopped(data) && compile_once(coder))
	{
		log_state(coder, "is debugging");
		if (!wait_ms(data, data->time_to_debug))
			break ;
		log_state(coder, "is refactoring");
		if (!wait_ms(data, data->time_to_refactor))
			break ;
	}
	return (NULL);
}

int	start_threads(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		if (pthread_create(&data->coder[i].thread,
				NULL, coder_routine, &data->coder[i]) != 0)
		{
			pthread_mutex_lock(&data->state_mutex);
			data->stop = 1;
			pthread_mutex_unlock(&data->state_mutex);
			join_threads(data, i);
			return (0);
		}
		i++;
	}
	return (1);
}
