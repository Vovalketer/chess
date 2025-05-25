#ifndef MOVE_H
#define MOVE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
	int x_src;
	int y_src;
	int x_dest;
	int y_dest;
} Move;

typedef struct {
	size_t size;
	size_t _capacity;
	Move *data;
} MoveList;

// returns a move by value, no need to free it
Move move_create(int x_src, int y_src, int x_dest, int y_dest);
bool move_list_create(MoveList **list);
void move_list_destroy(MoveList **list);
size_t move_list_size(const MoveList *list);
void move_list_clear(MoveList *list);
bool move_list_add(MoveList *list, Move move);
bool move_list_contains(const MoveList *list, Move move);
Move move_list_get(const MoveList *list, size_t index);
void move_list_remove(MoveList *list, size_t index);

#endif
