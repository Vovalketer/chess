#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "game_state.h"
#include "log.h"
#include "types.h"
#define FEN_VALID_CHARS_LEN 30
#define MAX_FEN_LEN 92

typedef struct {
	char* pieces;
	char* active_player;
	char* castling_rights;
	char* en_passant_target;
	char* halfmove_clock;
	char* fullmove_number;
	char _buffer[MAX_FEN_LEN];	// copy of the original fen
} ParsedFEN;

static Piece _char_to_piece(char c);
static bool _parse_to_ptr(const char* fen, ParsedFEN* pf_out);
static bool _parse_piece_placement(const char* pieces, GameState* state);
static bool _parse_active_player(const char* active_player, Player* p_out);
static bool _parse_castling_rights(const char* castling_rights, GameState* state);
static bool _parse_en_passant_target(const char* en_passant_target, GameState* state);
static bool _parse_halfmove_clock(const char* halfmove_clock, GameState* state);
static bool _parse_turn_number(const char* fullmove_counter, const char* active_player, GameState* state);
static bool _parse_str_to_int(const char* str, int* out);

static Piece _char_to_piece(char c) {
	switch (c) {
		case 'P':
			return (Piece) {.type = PAWN, .player = WHITE_PLAYER};
		case 'R':
			return (Piece) {.type = ROOK, .player = WHITE_PLAYER};
		case 'N':
			return (Piece) {.type = KNIGHT, .player = WHITE_PLAYER};
		case 'B':
			return (Piece) {.type = BISHOP, .player = WHITE_PLAYER};
		case 'Q':
			return (Piece) {.type = QUEEN, .player = WHITE_PLAYER};
		case 'K':
			return (Piece) {.type = KING, .player = WHITE_PLAYER};
		case 'p':
			return (Piece) {.type = PAWN, .player = BLACK_PLAYER};
		case 'r':
			return (Piece) {.type = ROOK, .player = BLACK_PLAYER};
		case 'n':
			return (Piece) {.type = KNIGHT, .player = BLACK_PLAYER};
		case 'b':
			return (Piece) {.type = BISHOP, .player = BLACK_PLAYER};
		case 'q':
			return (Piece) {.type = QUEEN, .player = BLACK_PLAYER};
		case 'k':
			return (Piece) {.type = KING, .player = BLACK_PLAYER};
		default:
			return (Piece) {.type = EMPTY, .player = NONE};
	}
}

bool fen_parse(const char* fen, GameState* state) {
	ParsedFEN pf = {0};
	if (!_parse_to_ptr(fen, &pf)) {
		return false;
	}
	bool piece_placement = _parse_piece_placement(pf.pieces, state);
	bool castling_rights = _parse_castling_rights(pf.castling_rights, state);
	bool en_passant_target = _parse_en_passant_target(pf.en_passant_target, state);
	bool halfmove_clock = _parse_halfmove_clock(pf.halfmove_clock, state);
	bool turn_number = _parse_turn_number(pf.fullmove_number, pf.active_player, state);

	return piece_placement && castling_rights && en_passant_target && halfmove_clock && turn_number;
}

static bool _parse_to_ptr(const char* fen, ParsedFEN* pf_out) {
	size_t fen_len = strlen(fen);
	if (fen_len > MAX_FEN_LEN) {
		log_error("FEN exceeded the max length of %d", MAX_FEN_LEN);
		return false;
	}

	strcpy(pf_out->_buffer, fen);

	char* cursor = pf_out->_buffer;
	char* fields[6] = {0};
	int field_index = 0;

	fields[field_index++] = cursor;
	while (*cursor && field_index < 6) {
		if (*cursor == ' ') {
			*cursor = '\0';
			fields[field_index++] = cursor + 1;
		}
		cursor++;
	}
	if (field_index != 6) {
		log_error("Error parsing FEN: Expected 5 spaces, got %d", field_index - 1);
		return false;
	}

	pf_out->pieces = fields[0];
	pf_out->active_player = fields[1];
	pf_out->castling_rights = fields[2];
	pf_out->en_passant_target = fields[3];
	pf_out->halfmove_clock = fields[4];
	pf_out->fullmove_number = fields[5];

	return true;
}

static bool _parse_piece_placement(const char* pieces, GameState* state) {
	int row = 0;
	int col = 0;
	while (*pieces) {
		if (*pieces == '/') {
			row++;
			col = 0;
			if (row > 7) {
				log_error("Error parsing pieces from FEN: invalid row - row: %d", row);
				return false;
			}
		} else if (*pieces >= '0' && *pieces <= '8') {
			col += *pieces - '0';
			if (col > 8) {
				log_error("Error parsing pieces from FEN: invalid column - row: %d, col: %d", row, col);
				return false;
			}
		} else {
			Piece p = _char_to_piece(*pieces);
			if (p.type == EMPTY) {
				log_error("Error parsing pieces from FEN: invalid piece - row: %d, col: %d, piece char:%c",
						  row,
						  col,
						  *pieces);
				return false;
			}
			if (!gstate_set_piece(state, p, (Position) {.x = col, .y = row})) {
				log_error(
					"Error parsing pieces from FEN: failed to set the piece in the board - row: %d, col: %d, "
					"piece char:%c",
					row,
					col,
					*pieces);
				return false;
			}
			col++;
		}
		pieces++;
	}
	return true;
}

static bool _parse_castling_rights(const char* castling_rights, GameState* state) {
	int cr_len = strlen(castling_rights);
	if (cr_len > 4) {
		log_error("Error parsing castling rights from FEN: invalid castling rights - %s", castling_rights);
		return false;
	}
	gstate_set_castling_rights_kingside(state, WHITE_PLAYER, false);
	gstate_set_castling_rights_queenside(state, WHITE_PLAYER, false);
	gstate_set_castling_rights_kingside(state, BLACK_PLAYER, false);
	gstate_set_castling_rights_queenside(state, BLACK_PLAYER, false);
	if (*castling_rights == '-') {
		return true;
	}

	const char* cursor = castling_rights;
	while (*cursor) {
		switch (*cursor) {
			case 'K':
				gstate_set_castling_rights_kingside(state, WHITE_PLAYER, true);
				break;
			case 'Q':
				gstate_set_castling_rights_queenside(state, WHITE_PLAYER, true);
				break;
			case 'k':
				gstate_set_castling_rights_kingside(state, BLACK_PLAYER, true);
				break;
			case 'q':
				gstate_set_castling_rights_queenside(state, BLACK_PLAYER, true);
				break;
			case '-':
				break;
			default:
				log_error("Error parsing castling rights from FEN: invalid castling rights - %s",
						  castling_rights);
				return false;
				break;
		}
		cursor++;
	}
	return true;
}

static bool _parse_en_passant_target(const char* en_passant_target, GameState* state) {
	int ep_len = strlen(en_passant_target);
	if (ep_len == 1) {
		if (en_passant_target[0] == '-') {
			gstate_set_en_passant_target(state, (Position) {-1, -1});
		} else {
			log_error("Error parsing en passant target from FEN: invalid en passant target - %s",
					  en_passant_target);
			return false;
		}
	} else if (ep_len == 2) {
		int col = tolower(en_passant_target[0]) - 'a';
		int row = en_passant_target[1] - '0' - 1;  // range 1-8, substract 1 to make it 0-7
		if (col < 0 || col > 7 || row < 0 || row > 7) {
			log_error("Error parsing en passant target from FEN: invalid en passant target. String: %s",
					  en_passant_target);
			return false;
		}
		log_info("Parsed en passant target from FEN: row: %d, col: %d", row, col);
		row = abs(row - 7);	 // invert the rows to match our coord system
		gstate_set_en_passant_target(state, (Position) {.x = col, .y = row});
	} else {
		log_error("Error parsing en passant target from FEN: invalid en passant target - %s",
				  en_passant_target);
		return false;
	}
	return true;
}

static bool _parse_halfmove_clock(const char* halfmove_clock, GameState* state) {
	int hmc;
	if (!_parse_str_to_int(halfmove_clock, &hmc)) {
		log_error("Error parsing halfmove clock from FEN: invalid halfmove clock - %s", halfmove_clock);
		return false;
	}
	gstate_set_halfmove_clock(state, hmc);
	return true;
}

static bool _parse_active_player(const char* active_player, Player* p_out) {
	if (strcmp(active_player, "w") == 0) {
		*p_out = WHITE_PLAYER;
	} else if (strcmp(active_player, "b") == 0) {
		*p_out = BLACK_PLAYER;
	} else {
		log_error("Error parsing active player from FEN: invalid active player - %s", active_player);
		return false;
	}
	return true;
}

static bool _parse_turn_number(const char* fullmove_counter, const char* active_player, GameState* state) {
	int fmc;
	Player p;
	if (!_parse_str_to_int(fullmove_counter, &fmc)) {
		log_error("Error parsing fullmove counter from FEN: invalid fullmove counter - %s", fullmove_counter);
		return false;
	}
	if (!_parse_active_player(active_player, &p)) {
		log_error("Error parsing active player from FEN: invalid active player - %s", active_player);
		return false;
	}

	if (p == WHITE_PLAYER) {
		gstate_set_turn(state, fmc * 2);
	} else {
		gstate_set_turn(state, fmc * 2 + 1);
	}
	return true;
}

static bool _parse_str_to_int(const char* str, int* out) {
	int res = 0;
	bool negative = false;
	if (*str == '-') {
		negative = true;
		str++;
	}
	while (*str) {
		if (*str >= '0' && *str <= '9') {
			res *= 10;
			res += *str - '0';
			str++;
		} else {
			log_error("Error parsing string to number");
			return false;
		}
	}
	*out = negative ? -res : res;
	return true;
}
