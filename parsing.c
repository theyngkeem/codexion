/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 16:41:03 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/17 10:36:19 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

int	ft_atoi(char *s)
{
	unsigned long	num;
	int				i;

	num = 0;
	i = 0;
	if (!s || !s[0])
		return (-1);
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (-1);
		if (num > (unsigned long)(INT_MAX - (s[i] - '0')) / 10)
			return (-1);
		num = num * 10 + (s[i] - '0');
		i++;
	}
	return ((int)num);
}

int	validate_args(char **av)
{
	int	i;

	i = 1;
	while (i <= 7)
	{
		if (ft_atoi(av[i]) == -1)
			return (0);
		i++;
	}
	if (strcmp(av[8], "fifo") != 0 && strcmp(av[8], "edf") != 0)
		return (0);
	return (1);
}

int	parse_args(int ac, char **av, t_data *data)
{
	if (ac != 9)
		return (0);
	if (!validate_args(av))
		return (0);
	data->number_of_coders = ft_atoi(av[1]);
	data->time_to_burnout = ft_atoi(av[2]);
	data->time_to_compile = ft_atoi(av[3]);
	data->time_to_debug = ft_atoi(av[4]);
	data->time_to_refactor = ft_atoi(av[5]);
	data->number_of_compiles_req = ft_atoi(av[6]);
	data->dongle_cooldown = ft_atoi(av[7]);
	if (strcmp(av[8], "fifo") == 0)
		data->scheduler = FIFO;
	else
		data->scheduler = EDF;
	return (1);
}
