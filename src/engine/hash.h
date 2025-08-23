#ifndef HASHING_H
#define HASHING_H

#include "types.h"

void	 hash_init(void);
uint64_t hash_board(Board* board);
void	 hash_update(Board* board, Move move, uint8_t old_castling_rights, Square old_ep_target);

#endif	// HASHING_H
