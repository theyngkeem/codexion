/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 09:21:16 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/17 22:10:45 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

void	cleanup_simulation(t_data *data)
{
	int i;
	
	i = 0;
	while (i < data->number_of_coders)
	{
		pthread_mutex_destroy(&data->dongle[i].mutex);
		i++;
	}   
	free(data->dongle);
    free(data->coder);
}

int main(int ac, char **av)
{
    t_data data;

    if (!parse_args(ac, av, &data))
        return (1);
    data.coder = malloc(sizeof(t_coder) * data.number_of_coders);
    if (!data.coder)
        return (1);
    data.dongle = malloc(sizeof(t_dongle) * data.number_of_coders);
    if (!data.dongle)
    {
        free(data.coder);
        return (1);
    }
    if (!ft_simulation(&data) || !init_dongles(&data))
    {
        free(data.dongle);
        free(data.coder);
        return (1);
    }
	cleanup_simulation(&data);
    return (0);
}
