/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 11:00:55 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/17 21:30:59 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

int init_dongles(t_data *data)
{
    int i;

    i = 0;
    while (i < data->number_of_coders)
    {
        data->dongle[i].in_use = 0;
        data->dongle[i].available_at = 0;
        if (pthread_mutex_init(&data->dongle[i].mutex, NULL) != 0)
        {
            while (--i >= 0)
                pthread_mutex_destroy(&data->dongle[i].mutex);
            return (0);
        }
        i++;
    }
    return (1);
}

int	ft_simulation(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->number_of_coders)
	{
		data->coder[i].id = i + 1;
		data->coder[i].complie_c = 0;
		data->coder[i].last_compile = 0;
		data->coder[i].left_dongel = i;
		data->coder[i].right_dongel = (i + 1) % data->number_of_coders;
		data->coder[i].data = data;
		i++;
	}
	return (1);
}
