#ifndef MATCH_H
#define MATCH_H

#include "history.h"
#include "position.h"
#include "turn_moves.h"
#include "types.h"

typedef enum { MATCH_IN_PROGRESS, MATCH_DRAW, MATCH_WHITE_WINS, MATCH_BLACK_WINS } MatchStatus;

// life cycle
bool match_create(MatchState **state);
bool match_create_empty(MatchState **state);
bool match_clone(MatchState **dst, const MatchState *src);
void match_destroy(MatchState **state);

// query
Piece match_get_piece(const MatchState *state, Position pos);
Board *match_get_board(MatchState *state);

// perform move
bool match_apply_move(MatchState *state, Move move, MoveType move_type);
bool match_undo_move(MatchState *state);

// legal moves
TurnMoves *match_get_legal_moves(MatchState *state);
void match_set_legal_moves(MatchState *state, TurnMoves *moves);

// castling rights
bool match_is_kingside_castling_available(MatchState *state, Player player);
bool match_is_queenside_castling_available(MatchState *state, Player player);

// turn
int match_get_turn(const MatchState *state);
int match_previous_turn(MatchState *state);
int match_next_turn(MatchState *state);
Player match_get_player_turn(const MatchState *state);

// promotion
void match_set_next_promotion_type(MatchState *state, Player player, PromotionType type);

// match status
MatchStatus match_get_status(MatchState *state);
void match_set_status(MatchState *state, MatchStatus status);

// history
TurnHistory *match_get_history(MatchState *state);
bool match_get_turn_record(MatchState *state, size_t turn, TurnRecord **out_record);
bool match_get_last_turn_record(MatchState *state, TurnRecord **out_record);
bool match_get_history_clone(MatchState *state, TurnHistory **out_history);

#endif
