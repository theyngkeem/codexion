/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleaner.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 02:17:28 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/26 20:50:58 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

void	cleanup_simulation(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		pthread_cond_destroy(&data->dongle[i].cond);
		pthread_mutex_destroy(&data->dongle[i].mutex);
		i++;
	}
	free(data->dongle);
	free(data->coder);
}

void	cleanup_scheduler(t_data *data)
{
	pthread_cond_destroy(&data->sch_cond);
	pthread_mutex_destroy(&data->sch_mutex);
	free_heap(data->waiting);
	data->waiting = NULL;
}

void	cleanup_shared_state(t_data *data)
{
	pthread_mutex_destroy(&data->log_mutex);
	pthread_mutex_destroy(&data->state_mutex);
}
