#ifndef MATCH_H
#define MATCH_H

#include "history.h"
#include "position.h"
#include "turn_moves.h"
#include "types.h"

typedef enum { MATCH_IN_PROGRESS, MATCH_DRAW, MATCH_WHITE_WINS, MATCH_BLACK_WINS } MatchStatus;

// life cycle
bool gstate_create(GameState **state);
bool gstate_create_empty(GameState **state);
bool gstate_clone(GameState **dst, const GameState *src);
void gstate_destroy(GameState **state);

// query
Piece gstate_get_piece(const GameState *state, Position pos);
Board *gstate_get_board(GameState *state);
int gstate_get_halfmove_clock(GameState *state);

// perform move
bool gstate_apply_move(GameState *state, Move move, MoveType move_type);
bool gstate_undo_move(GameState *state);

// legal moves
TurnMoves *gstate_get_legal_moves(GameState *state);
void gstate_set_legal_moves(GameState *state, TurnMoves *moves);

// castling rights
bool gstate_is_kingside_castling_available(GameState *state, Player player);
bool gstate_is_queenside_castling_available(GameState *state, Player player);

// turn
int gstate_get_turn(const GameState *state);
int gstate_previous_turn(GameState *state);
int gstate_next_turn(GameState *state);
Player gstate_get_player_turn(const GameState *state);

// promotion
void gstate_set_next_promotion_type(GameState *state, Player player, PromotionType type);

// match status
MatchStatus gstate_get_status(GameState *state);
void gstate_set_status(GameState *state, MatchStatus status);

// history
TurnHistory *gstate_get_history(GameState *state);
bool gstate_get_turn_record(GameState *state, size_t turn, TurnRecord **out_record);
bool gstate_get_last_turn_record(GameState *state, TurnRecord **out_record);
bool gstate_get_history_clone(GameState *state, TurnHistory **out_history);

#endif
