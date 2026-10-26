/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yaarab <yaarab@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/16 14:56:21 by yaarab            #+#    #+#             */
/*   Updated: 2026/10/26 20:50:58 by yaarab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEX_H
# define CODEX_H

# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
#include <sys/time.h>
# include <unistd.h>


typedef struct s_data t_data;
typedef struct s_coder t_coder;
typedef enum e_scheduler
{
	FIFO,
	EDF
}				t_scheduler;

typedef struct s_request
{
	int				coder_id;
	long			deadline;
	unsigned long	sequence;
}	t_request;


typedef struct s_heap
{
    t_request       *arr;
    int             size;
    int             capacity;
    t_scheduler     scheduler;
}   t_heap;


typedef struct s_dongle
{
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    int             in_use;
    long            available_at;
}   t_dongle;



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
    pthread_mutex_t	state_mutex;
    pthread_mutex_t	log_mutex;
	t_heap			*waiting;
	pthread_mutex_t	sch_mutex;
	pthread_cond_t	sch_cond;
    long			start_time;
    int			stop;
	unsigned long   request_seq;
}				t_data;

typedef struct s_coder
{
	int			id;
	pthread_t	thread;
	long		last_compile;
	int			complie_c;
	int 		left_dongel;
	int 		right_dongel;
	int	request_pending;
	int	dongles_granted;
	t_data		*data;
}				t_coder;

int     parse_args(int ac, char **av, t_data *data);
void    init_coder(t_data *data);
int     init_dongles(t_data *data);
void	cleanup_simulation(t_data *data);
void    cleanup_scheduler(t_data *data);
void	cleanup_shared_state(t_data *data);

int		start_threads(t_data *data);
void	join_threads(t_data *data, int count);
long	now_ms(void);
int		init_shared_state(t_data *data);
int		simulation_stopped(t_data *data);
int		wait_ms(t_data *data, long duration);
void	log_state(t_coder *coder, const char *message);
void	count_compile(t_coder *coder);
t_heap   *creat_heap(int capacity, t_scheduler scheduler);
int      push_heap(t_heap *heap, t_request req);
int      heap_pop(t_heap *heap, t_request *res);
void     free_heap(t_heap *heap);
int init_scheduler(t_data *data);
int init_shared_state(t_data *data);
#endif
