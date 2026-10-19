/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 14:56:21 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/17 21:45:21 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEX_H
# define CODEX_H

# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

typedef struct s_data t_data;
typedef struct s_coder t_coder;

typedef struct s_dongle
{
    pthread_mutex_t mutex;
    int in_use;
    long available_at;
} t_dongle;


typedef enum e_scheduler
{
	FIFO,
	EDF
}				t_scheduler;

typedef struct s_data
{
	int			number_of_coders;
	int			time_to_burnout;
	int			time_to_compile;
	int			time_to_debug;
	int			time_to_refactor;
	int			number_of_compiles_req;
	int			dongle_cooldown;
	t_scheduler	scheduler;
    t_coder *coder;
    t_dongle *dongle;
}				t_data;

typedef struct s_coder
{
	int			id;
	pthread_t	thread;
	long		last_compile;
	int			complie_c;
	int 		left_dongel;
	int 		right_dongel;
	t_data		*data;
}				t_coder;

int     parse_args(int ac, char **av, t_data *data);
int     ft_simulation(t_data *data);
int     init_dongles(t_data *data);
void	cleanup_simulation(t_data *data);

#endif