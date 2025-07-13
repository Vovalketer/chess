#ifndef RULES_H
#define RULES_H
#include "../include/move.h"
#include "turn_moves.h"
#include "types.h"

bool rules_is_check(GameState *state, Player player);
bool rules_is_check_after_move(GameState *state, Player player, Move move);
TurnMoves *rules_generate_turn_moves(GameState *state, Player player);
bool rules_is_checkmate(GameState *state, Player player);
bool rules_is_fifty_moves_draw(GameState *state);
bool rules_is_stalemate(GameState *state);
bool rules_is_promotion(GameState *state, Move move);
bool rules_is_castling(GameState *state, Move move);
bool rules_is_en_passant(GameState *state, Move move);
bool rules_can_castle_kingside(GameState *state, Player player);
bool rules_can_castle_queenside(GameState *state, Player player);
MoveType rules_get_move_type(GameState *state, Move move);

#endif
