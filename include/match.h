#ifndef MATCH_H
#define MATCH_H

#include "history.h"
#include "position.h"
#include "turn_moves.h"
#include "types.h"

typedef enum { MATCH_IN_PROGRESS, MATCH_DRAW, MATCH_WHITE_WINS, MATCH_BLACK_WINS } MatchStatus;

bool match_create(MatchState **state);
bool match_create_empty(MatchState **state);
bool match_clone(MatchState **dst, const MatchState *src);
void match_destroy(MatchState **state);
Board *match_get_board(MatchState *state);
MatchStatus match_get_status(MatchState *state);
void match_set_status(MatchState *state, MatchStatus status);
bool match_move_piece(MatchState *state, Move move);
bool match_move_promotion(MatchState *state, Move move);
Piece match_get_piece(const MatchState *state, Position pos);
void match_set_next_promotion_type(MatchState *state, Player player, PromotionType type);
Player match_get_enemy_player(Player player);
Player match_get_player_turn(const MatchState *state);
int match_get_turn(const MatchState *state);
int match_previous_turn(MatchState *state);
int match_next_turn(MatchState *state);
bool match_get_turn_record(MatchState *state, size_t turn, TurnRecord **out_record);
bool match_get_last_turn_record(MatchState *state, TurnRecord **out_record);
TurnHistory *match_get_history(MatchState *state);
// Returns a clone of the history. User is in charge of freeing the memory
bool match_get_history_clone(MatchState *state, TurnHistory **out_history);
bool match_undo_move(MatchState *state);
bool match_is_kingside_castling_available(MatchState *state, Player player);
bool match_is_queenside_castling_available(MatchState *state, Player player);
bool match_move_castling(MatchState *state, Move move);
bool match_move_en_passant(MatchState *state, Move move);
TurnMoves *match_get_legal_moves(MatchState *state);
void match_set_legal_moves(MatchState *state, TurnMoves *moves);
#endif
