/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threades.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 22:10:25 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/26 20:52:08 by yaarab           ###   ########.fr       */
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

	coder = (t_coder *)arg;
	log_state(coder, "is refactoring");
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
