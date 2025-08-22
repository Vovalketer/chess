#include "fen.h"

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "board.h"
#include "log.h"
#include "types.h"
#include "utils.h"
#define FEN_VALID_CHARS_LEN 30
#define MAX_FEN_LEN			92

typedef struct {
	char* pieces;
	char* active_player;
	char* castling_rights;
	char* en_passant_target;
	char* halfmove_clock;
	char* fullmove_counter;
	char  _buffer[MAX_FEN_LEN];	 // copy of the original fen
} ParsedFEN;

static bool _parse_to_ptr(const char* fen, ParsedFEN* pf_out);
static bool _parse_piece_placement(const char* pieces, Board* board);
static bool _parse_active_player(const char* active_player, Board* board);
static bool _parse_castling_rights(const char* castling_rights, Board* board);
static bool _parse_en_passant_target(const char* en_passant_target, Board* board);
static bool _parse_halfmove_clock(const char* halfmove_clock, Board* board);
static bool _parse_fullmove_counter(const char* fullmove_counter, Board* board);
static bool _parse_str_to_int(const char* str, int* out);

bool fen_parse(const char* fen, Board* board) {
	ParsedFEN pf = {0};
	if (!_parse_to_ptr(fen, &pf)) {
		return false;
	}
	bool piece_placement   = _parse_piece_placement(pf.pieces, board);
	bool castling_rights   = _parse_castling_rights(pf.castling_rights, board);
	bool en_passant_target = _parse_en_passant_target(pf.en_passant_target, board);
	bool active_player	   = _parse_active_player(pf.active_player, board);
	bool halfmove_clock	   = _parse_halfmove_clock(pf.halfmove_clock, board);
	bool fullmove_counter  = _parse_fullmove_counter(pf.fullmove_counter, board);

	return piece_placement && castling_rights && en_passant_target && active_player &&
		   halfmove_clock && fullmove_counter;
}

static bool _parse_to_ptr(const char* fen, ParsedFEN* pf_out) {
	size_t fen_len = strlen(fen);
	if (fen_len > MAX_FEN_LEN) {
		log_error("FEN exceeded the max length of %d", MAX_FEN_LEN);
		return false;
	}

	strcpy(pf_out->_buffer, fen);

	char* cursor	  = pf_out->_buffer;
	char* fields[6]	  = {0};
	int	  field_index = 0;

	fields[field_index++] = cursor;
	while (*cursor && field_index < 6) {
		if (*cursor == ' ') {
			*cursor				  = '\0';
			fields[field_index++] = cursor + 1;
		}
		cursor++;
	}
	if (field_index != 6) {
		log_error("Error parsing FEN: Expected 5 spaces, got %d", field_index - 1);
		return false;
	}

	pf_out->pieces			  = fields[0];
	pf_out->active_player	  = fields[1];
	pf_out->castling_rights	  = fields[2];
	pf_out->en_passant_target = fields[3];
	pf_out->halfmove_clock	  = fields[4];
	pf_out->fullmove_counter  = fields[5];

	return true;
}

static bool _parse_piece_placement(const char* pieces, Board* board) {
	int rank = 7;
	int file = 0;
	while (*pieces) {
		if (*pieces == '/') {
			rank--;
			file = 0;
			if (rank > 7) {
				log_error("Error parsing pieces from FEN: invalid row - row: %d", rank);
				return false;
			}
		} else if (*pieces >= '0' && *pieces <= '8') {
			file += *pieces - '0';
			if (file > 8) {
				log_error(
					"Error parsing pieces from FEN: invalid column - row: %d, col: %d", rank, file);
				return false;
			}
		} else {
			Piece p = utils_char_to_piece(*pieces);
			if (p.type == EMPTY) {
				log_error(
					"Error parsing pieces from FEN: invalid piece - row: %d, col: %d, piece "
					"char:%c",
					rank,
					file,
					*pieces);
				return false;
			}
			board_set_piece(board,
							(Piece) {.player = p.player, .type = p.type},
							utils_fr_to_square(file, rank));
			file++;
		}
		pieces++;
	}
	return true;
}

static bool _parse_castling_rights(const char* castling_rights, Board* board) {
	int cr_len = strlen(castling_rights);
	if (cr_len > 4) {
		log_error("Error parsing castling rights from FEN: invalid castling rights - %s",
				  castling_rights);
		return false;
	}
	board_remove_castling_rights(board, CASTLING_ALL_RIGHTS);
	if (*castling_rights == '-') {
		return true;
	}

	const char* cursor = castling_rights;
	while (*cursor) {
		switch (*cursor) {
			case 'K':
				board_set_castling_rights(board, CASTLING_WHITE_KS);
				break;
			case 'Q':
				board_set_castling_rights(board, CASTLING_WHITE_QS);
				break;
			case 'k':
				board_set_castling_rights(board, CASTLING_BLACK_KS);
				break;
			case 'q':
				board_set_castling_rights(board, CASTLING_BLACK_QS);
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

static bool _parse_en_passant_target(const char* en_passant_target, Board* board) {
	int ep_len = strlen(en_passant_target);
	if (ep_len == 1) {
		if (en_passant_target[0] == '-') {
			board->ep_target = SQ_NONE;
		} else {
			log_error("Error parsing en passant target from FEN: invalid en passant target - %s",
					  en_passant_target);
			return false;
		}
	} else if (ep_len == 2) {
		int file = tolower(en_passant_target[0]) - 'a';
		int rank = en_passant_target[1] - '1';	// range 1-8, substract 1 to make it 0-7
		if (file < 0 || file > 7 || rank < 0 || rank > 7) {
			log_error(
				"Error parsing en passant target from FEN: invalid en passant target. String: %s",
				en_passant_target);
			return false;
		}
		board->ep_target = utils_fr_to_square(file, rank);
	} else {
		log_error("Error parsing en passant target from FEN: invalid en passant target - %s",
				  en_passant_target);
		return false;
	}
	return true;
}

static bool _parse_halfmove_clock(const char* halfmove_clock, Board* board) {
	int hmc;
	if (!_parse_str_to_int(halfmove_clock, &hmc)) {
		log_error("Error parsing halfmove clock from FEN: invalid halfmove clock - %s",
				  halfmove_clock);
		return false;
	}
	board->halfmove_clock = hmc;
	return true;
}

static bool _parse_active_player(const char* active_player, Board* board) {
	if (strcmp(active_player, "w") == 0) {
		board->side = PLAYER_W;
	} else if (strcmp(active_player, "b") == 0) {
		board->side = PLAYER_B;
	} else {
		log_error("Error parsing active player from FEN: invalid active player - %s",
				  active_player);
		return false;
	}
	return true;
}

static bool _parse_fullmove_counter(const char* fullmove_counter, Board* board) {
	int fmc;
	if (!_parse_str_to_int(fullmove_counter, &fmc)) {
		log_error("Error parsing fullmove counter from FEN: invalid fullmove counter - %s",
				  fullmove_counter);
		return false;
	}
	board->fullmove_counter = fmc;
	return true;
}

static bool _parse_str_to_int(const char* str, int* out) {
	int	 res	  = 0;
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

FenString fen_from_board(const Board* board) {
	FenString fen = {0};
	char	  pieces[8][8];
	char	  side[2];
	char	  castling_rights[5];
	char	  en_passant_target[3];
	int		  halfmove_clock;
	int		  fullmove_counter;

	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			Piece p				 = board_get_piece(board, utils_fr_to_square(col, row));
			pieces[7 - row][col] = utils_piece_to_char(p);
		}
	}
	log_info("printing pieces");
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			printf("%c", pieces[i][j]);
		}
	}
	printf("\n");
	side[0] = board->side == PLAYER_W ? 'w' : 'b';
	side[1] = '\0';

	int fen_idx = 0;
	if (board_has_castling_rights(board, CASTLING_WHITE_KS)) {
		castling_rights[fen_idx++] = 'K';
	}
	if (board_has_castling_rights(board, CASTLING_WHITE_QS)) {
		castling_rights[fen_idx++] = 'Q';
	}
	if (board_has_castling_rights(board, CASTLING_BLACK_KS)) {
		castling_rights[fen_idx++] = 'k';
	}
	if (board_has_castling_rights(board, CASTLING_BLACK_QS)) {
		castling_rights[fen_idx++] = 'q';
	}
	if (fen_idx == 0) {
		castling_rights[fen_idx++] = '-';
	}
	castling_rights[fen_idx] = '\0';

	if (board->ep_target == SQ_NONE) {
		en_passant_target[0] = '-';
		en_passant_target[1] = '\0';
	} else {
		en_passant_target[0] = utils_get_file(board->ep_target) + 'a';
		en_passant_target[1] = utils_get_rank(board->ep_target) + '1';
		en_passant_target[2] = '\0';
	}

	halfmove_clock	 = board->halfmove_clock;
	fullmove_counter = board->fullmove_counter;

	const char empty_square_char =
		utils_piece_to_char((Piece) {.player = PLAYER_NONE, .type = EMPTY});
	fen_idx = 0;
	for (int row = 0; row < 8; row++) {
		int empty_squares = 0;
		for (int col = 0; col < 8; col++) {
			if (col == 0 && row != 0) {
				fen.str[fen_idx++] = '/';
			}
			if (pieces[row][col] != empty_square_char) {
				if (empty_squares > 0) {
					fen.str[fen_idx++] = empty_squares + '0';
					empty_squares	   = 0;
				}
				fen.str[fen_idx++] = pieces[row][col];
			} else {
				empty_squares++;
				if (col == 7) {
					fen.str[fen_idx++] = empty_squares + '0';
				}
			}
		}
	}
	fen.str[fen_idx++] = ' ';

	char* ptr = side;
	while (*ptr) {
		fen.str[fen_idx++] = *ptr++;
	}
	fen.str[fen_idx++] = ' ';

	ptr = castling_rights;
	while (*ptr) {
		fen.str[fen_idx++] = *ptr++;
	}
	fen.str[fen_idx++] = ' ';

	ptr = en_passant_target;
	while (*ptr) {
		fen.str[fen_idx++] = *ptr++;
	}
	fen.str[fen_idx++] = ' ';

	fen.str[fen_idx++] = halfmove_clock + '0';
	fen.str[fen_idx++] = ' ';

	fen.str[fen_idx++] = fullmove_counter + '0';
	fen.str[fen_idx++] = '\0';

	return fen;
}
