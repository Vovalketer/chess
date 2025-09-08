#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "bitboards.h"
#include "board.h"
#include "log.h"
#include "makemove.h"
#include "movegen.h"
#include "types.h"
#include "utils.h"

typedef struct {
	Board *board;
	int	   depth;
	Square from;
	Square to;
} PerftTask;

typedef struct {
	Square	 from;
	Square	 to;
	uint64_t nodes;
} PerftResult;

uint64_t perft(Board *board, int depth);
uint64_t perft_divide(Board *board, int depth);
uint64_t perft_divide_parallel(Board *board, int depth, int threads_num);
int		 threads_count = 4;

int main(int argc, char *argv[]) {
	struct timeval start, end;
	if (argc < 3) {
		printf("Usage: %s \"fen\" depth threads(deault: 4)\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	Board *board = board_create();
	bitboards_init();
	if (!board_from_fen(board, argv[1])) {
		printf("Error parsing FEN\n");
		exit(EXIT_FAILURE);
	}
	if (argv[3])
		threads_count = atoi(argv[3]);
	log_info("Using threads: %d\n", threads_count);

	uint64_t total;
	gettimeofday(&start, 0);  // start timer
	if (threads_count == 1)
		total = perft(board, atoi(argv[2]));
	else
		total = perft_divide_parallel(board, atoi(argv[2]), threads_count);
	printf("Total nodes: %lu\n", total);

	gettimeofday(&end, 0);	// stop timer
	board_destroy(&board);

	long   seconds	= end.tv_sec - start.tv_sec;
	long   useconds = end.tv_usec - start.tv_usec;
	double elapsed	= (double) seconds + (double) useconds / 1000000;
	printf("Elapsed time: %f\n", elapsed);

	return EXIT_SUCCESS;
}

uint64_t perft(Board *board, int depth) {
	if (depth == 0) {
		return 1;
	}
	uint64_t  nodes = 0;
	MoveList *ml	= movegen_generate(board, board_get_player_turn(board));
	for (size_t i = 0; i < move_list_size(ml); i++) {
		Move *m = move_list_at(ml, i);
		if (make_move(board, *m)) {
			nodes += perft(board, depth - 1);
			unmake_move(board);
		}
	}

	move_list_destroy(&ml);
	return nodes;
}

uint64_t perft_divide(Board *board, int depth) {
	MoveList *ml	= movegen_generate(board, board_get_player_turn(board));
	uint64_t  total = 0;

	for (size_t i = 0; i < move_list_size(ml); i++) {
		Move m = *move_list_at(ml, i);

		if (!make_move(board, m))
			continue;

		uint64_t nodes = perft(board, depth - 1);
		unmake_move(board);

		printf("%s%s: %lu\n", utils_square_to_str(m.from), utils_square_to_str(m.to), nodes);
		total += nodes;
	}
	move_list_destroy(&ml);
	return total;
}

// fixed arrays, there's a limit of 16 pieces per side so it'll never overflow
// given that we're splitting at depth 1 and we have a maximum of 16 pieces
PerftTask	   *tasks[32];
int				tasks_count = 0;
PerftResult		tasks_done[32];
int				tasks_done_count = 0;
pthread_mutex_t mtx_queue;
pthread_mutex_t mtx_done;
pthread_cond_t	cond_queue;
bool			perft_done = false;

void perft_add_task(Board *board, Square from, Square to, int depth) {
	pthread_mutex_lock(&mtx_queue);
	PerftTask *t		 = malloc(sizeof(PerftTask));
	t->board			 = board_clone(board);
	t->from				 = from;
	t->to				 = to;
	t->depth			 = depth;
	tasks[tasks_count++] = t;
	pthread_mutex_unlock(&mtx_queue);
	pthread_cond_signal(&cond_queue);
}

void perft_finish_task(PerftTask *task, uint64_t nodes) {
	board_destroy(&task->board);
	pthread_mutex_lock(&mtx_done);
	tasks_done[tasks_done_count++] = (PerftResult) {task->from, task->to, nodes};
	pthread_mutex_unlock(&mtx_done);
	free(task);
}

void *perft_exec_task(void *arg) {
	(void) arg;	 // unused but required by pthread_create
	while (1) {
		pthread_mutex_lock(&mtx_queue);
		while (tasks_count == 0 && !perft_done) {
			pthread_cond_wait(&cond_queue, &mtx_queue);
		}
		if (perft_done && tasks_count == 0) {
			pthread_mutex_unlock(&mtx_queue);
			return NULL;
		}
		PerftTask *task = tasks[--tasks_count];
		pthread_mutex_unlock(&mtx_queue);

		uint64_t nodes = perft(task->board, task->depth);
		perft_finish_task(task, nodes);
	}
}

void perft_init_threads(pthread_t *th, int threads_num) {
	pthread_mutex_init(&mtx_queue, NULL);
	pthread_mutex_init(&mtx_done, NULL);
	pthread_cond_init(&cond_queue, NULL);
	for (int i = 0; i < threads_num; i++) {
		pthread_create(&th[i], NULL, perft_exec_task, NULL);
	}
}

void perft_join_threads(pthread_t *th, int threads_num) {
	perft_done = true;
	pthread_cond_broadcast(&cond_queue);
	for (int i = 0; i < threads_num; i++) {
		if (pthread_join(th[i], NULL)) {
			log_error("Failed to join thread %d", i);
			exit(EXIT_FAILURE);
		}
	}
	pthread_mutex_destroy(&mtx_queue);
	pthread_mutex_destroy(&mtx_done);
	pthread_cond_destroy(&cond_queue);
}

uint64_t perft_divide_parallel(Board *board, int depth, int threads_num) {
	uint64_t  total	 = 0;
	pthread_t th[32] = {0};
	perft_init_threads(th, threads_num);

	MoveList *ml = movegen_generate(board, board_get_player_turn(board));

	for (size_t i = 0; i < move_list_size(ml); i++) {
		Move *m = move_list_at(ml, i);
		if (!make_move(board, *m))
			continue;
		perft_add_task(board, m->from, m->to, depth - 1);
		unmake_move(board);
	}

	perft_join_threads(th, threads_num);

	for (int i = 0; i < tasks_done_count; i++) {
		PerftResult res = tasks_done[i];
		printf(
			"%s%s: %lu\n", utils_square_to_str(res.from), utils_square_to_str(res.to), res.nodes);
		total += res.nodes;
	}
	return total;
}
