/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/17 09:21:16 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/26 20:50:58 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

long	now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

int	main(int ac, char **av)
{
	t_data	data;

	memset(&data, 0, sizeof(t_data));
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
	if (!init_dongles(&data))
	{
		free(data.dongle);
		free(data.coder);
		return (1);
	}
	init_coder(&data);
	if (!init_scheduler(&data))
	{
		cleanup_simulation(&data);
		return (1);
	}
	if (!init_shared_state(&data))
	{
		cleanup_scheduler(&data);
		cleanup_simulation(&data);
		return (1);
	}
	data.start_time = now_ms();
	if (!start_threads(&data))
	{
		cleanup_shared_state(&data);
		cleanup_scheduler(&data);
		cleanup_simulation(&data);
		return (1);
	}
	join_threads(&data, data.number_of_coders);
	cleanup_shared_state(&data);
	cleanup_scheduler(&data);
	cleanup_simulation(&data);
	return (0);
}
