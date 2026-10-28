NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRC = codex.c parsing.c simulation.c dongle_init.c cleaner.c \
	state.c threades.c monitor.c scheduler.c scheduler_request.c \
	dongles.c heap.c heap_remove.c heap_pop.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c codex.h Makefile
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean
	make all

.PHONY: all clean fclean re
