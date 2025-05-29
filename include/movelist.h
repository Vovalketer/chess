#ifndef MOVELIST_H
#define MOVELIST_H
#include <stddef.h>

#include "move.h"

typedef struct MoveList MoveList;

bool move_list_create(MoveList **list);
void move_list_destroy(MoveList **list);
size_t move_list_size(const MoveList *list);
void move_list_clear(MoveList *list);
bool move_list_add(MoveList *list, Move move);
bool move_list_contains(const MoveList *list, Move move);
Move move_list_get(const MoveList *list, size_t index);
void move_list_remove(MoveList *list, size_t index);
Move move_list_pop(MoveList *list);
#endif
