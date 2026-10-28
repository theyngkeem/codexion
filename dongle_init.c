/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 09:21:16 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/28 05:37:01 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

static int	destroy_initialized(t_data *data, int i)
{
	while (--i >= 0)
	{
		pthread_cond_destroy(&data->dongle[i].cond);
		pthread_mutex_destroy(&data->dongle[i].mutex);
	}
	return (0);
}

int	init_dongles(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		data->dongle[i].in_use = 0;
		data->dongle[i].available_at = 0;
		if (pthread_mutex_init(&data->dongle[i].mutex, NULL) != 0)
			return (destroy_initialized(data, i));
		if (pthread_cond_init(&data->dongle[i].cond, NULL) != 0)
		{
			pthread_mutex_destroy(&data->dongle[i].mutex);
			return (destroy_initialized(data, i));
		}
		i++;
	}
	return (1);
}
