#ifndef FEN_H
#define FEN_H
#include "../include/types.h"
#define FEN_MAX_LENGTH	96
#define FEN_TOKEN_COUNT 6

bool	  fen_parse(const char *fen, Board *board);
FenString fen_from_board(const Board *board);

#endif
