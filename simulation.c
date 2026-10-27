/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 11:00:55 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/26 23:14:36 by yaarab           ###   ########.fr       */
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
            {
                pthread_cond_destroy(&data->dongle[i].cond);
                pthread_mutex_destroy(&data->dongle[i].mutex);
            }
            return (0);
        }
        if (pthread_cond_init(&data->dongle[i].cond, NULL) != 0)
        {
            pthread_mutex_destroy(&data->dongle[i].mutex);
            while (--i >= 0)
            {
                pthread_cond_destroy(&data->dongle[i].cond);
                pthread_mutex_destroy(&data->dongle[i].mutex);
            }
            return (0);
        }
        i++;
    }
    return (1);
}

void    init_coder(t_data *data)
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
        data->coder[i].request_pending = 0;
        data->coder[i].dongles_granted = 0;
		i++;
	}
}

int init_scheduler(t_data *data)
{
    data->waiting = creat_heap(data->number_of_coders, data->scheduler);
	data->request_seq = 0;
    if (!data->waiting)
        return 0;
    if (pthread_mutex_init(&data->sch_mutex, NULL) != 0)
    {
        free_heap(data->waiting);
        data->waiting = NULL;
        return (0);
    }
    if (pthread_cond_init(&data->sch_cond, NULL) != 0)
    {
        pthread_mutex_destroy(&data->sch_mutex);
        free_heap(data->waiting);
        data->waiting = NULL;
        return (0);
    }
    return (1);
}

int	init_shared_state(t_data *data)
{
	if (pthread_mutex_init(&data->state_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&data->log_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->state_mutex);
		return (0);
	}
	data->stop = 0;
	return (1);
}

void	count_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->data->state_mutex);
	coder->complie_c++;
	pthread_mutex_unlock(&coder->data->state_mutex);
}
