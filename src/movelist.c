#include "../include/movelist.h"

#include <assert.h>
#include <stdlib.h>

#define LIST_INITIAL_CAPACITY 16

struct MoveList {
	size_t size;
	size_t capacity;
	Move *data;
};

static bool _position_eq(Position p1, Position p2) {
	return p1.x == p2.x && p1.y == p2.y;
}

bool move_list_create(MoveList **list) {
	assert(list != NULL);
	MoveList *moves = malloc(sizeof(MoveList));
	if (moves == NULL) {
		return false;
	}
	moves->data = malloc(sizeof(Move) * LIST_INITIAL_CAPACITY);
	if (moves->data == NULL) {
		free(moves);
		return false;
	}
	moves->size = 0;
	moves->capacity = LIST_INITIAL_CAPACITY;
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

void move_list_clear(MoveList *list) {
	assert(list != NULL);
	list->size = 0;
}

size_t move_list_size(const MoveList *list) {
	assert(list != NULL);
	return list->size;
}

bool move_list_add(MoveList *list, Move move) {
	assert(list->size <= list->capacity);

	if (list->size == list->capacity) {
		list->capacity *= 2;
		Move *d = realloc(list->data, sizeof(Move) * list->capacity);
		if (d == NULL) {
			return false;
		}
		list->data = d;
	}
	list->data[list->size] = move;
	list->size++;
	return true;
}

bool move_list_contains(const MoveList *list, Move move) {
	assert(list != NULL);
	for (unsigned int i = 0; i < list->size; i++) {
		Move m = list->data[i];
		if (_position_eq(m.src, move.src) && _position_eq(m.dst, move.dst)) {
			return true;
		}
	}
	return false;
}

Move move_list_get(const MoveList *list, size_t index) {
	assert(list != NULL);
	assert(index < list->size);
	return list->data[index];
}

void move_list_remove(MoveList *list, size_t index) {
	assert(list != NULL);
	assert(index < list->size);
	for (size_t i = index; i < list->size - 1; i++) {
		list->data[i] = list->data[i + 1];
	}
	list->size--;
}

Move move_list_pop(MoveList *list) {
	assert(list != NULL);
	assert(list->size > 0);
	Move move = move_list_get(list, list->size - 1);
	move_list_remove(list, list->size - 1);
	return move;
}
