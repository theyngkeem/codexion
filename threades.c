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

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_data	*data;

	coder = (t_coder *)arg;
	data = coder->data;
	while (!simulation_stopped(data))
	{
		if (!wait_for_dongles(coder))
			break ;
		log_state(coder, "has taken a dongle");
		log_state(coder, "has taken a dongle");
		pthread_mutex_lock(&data->state_mutex);
		coder->last_compile = now_ms();
		pthread_mutex_unlock(&data->state_mutex);
		log_state(coder, "is compiling");
		if (!wait_ms(data, data->time_to_compile))
		{
			release_dongles(coder);
			break ;
		}
		count_compile(coder);
		release_dongles(coder);
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
			while (--i >= 0)
				pthread_join(data->coder[i].thread, NULL);
			return (0);
		}
		i++;
	}
	return (1);
}
