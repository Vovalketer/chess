#include "../include/move.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

Move move_create(int x_src, int y_src, int x_dest, int y_dest) {
	return (Move) {x_src, y_src, x_dest, y_dest};
}

bool move_list_create(MoveList **list) {
	if (!list) {
		return false;
	}
	unsigned int initial_capacity = 8;
	MoveList *moves = malloc(sizeof(MoveList));
	if (moves == NULL) {
		return false;
	}
	moves->data = malloc(sizeof(Move) * initial_capacity);
	if (moves->data == NULL) {
		free(moves);
		return false;
	}
	moves->size = 0;
	moves->_capacity = initial_capacity;
	*list = moves;
	return true;
}

void move_list_destroy(MoveList **list) {
	if (list && *list != NULL) {
		free((*list)->data);
		free(*list);
		*list = NULL;
	}
}

bool move_list_clear(MoveList *list) {
	list->size = 0;
	return true;
}

size_t move_list_size(const MoveList *list) {
	return list->size;
}

// list cant contain more than COORD_LIST_MAX_SIZE moves
bool move_list_add(MoveList *list, Move move) {
	// this case should never happen under normal circumstances unless the
	// user has modified the _capacity
	if (list->size > list->_capacity) {
		return false;
	}
	if (list->size == list->_capacity) {
		list->_capacity *= 2;
		list->data = realloc(list->data, sizeof(Move) * list->_capacity);
	}
	list->data[list->size] = move;
	list->size++;
	return true;
}

bool move_list_contains(const MoveList *list, Move move) {
	assert(list != NULL);
	for (unsigned int i = 0; i < list->size; i++) {
		Move m = list->data[i];
		if (m.x_src == move.x_src && m.y_src == move.y_src && m.x_dest == move.x_dest &&
			m.y_dest == move.y_dest) {
			return true;
		}
	}
	return false;
}

// returns a new instance of a move if found
bool move_list_get(const MoveList *list, size_t index, Move *out) {
	assert(list != NULL || index < list->size);
	if (index >= list->size) {
		return false;
	}
	*out = list->data[index];

	return true;
}

bool move_list_remove(MoveList *list, size_t index) {
	if (!list || index >= list->size) {
		return false;
	}
	for (unsigned int i = index; i < list->size; i++) {
		list->data[i] = list->data[i + 1];
	}
	list->size--;
	return true;
}
