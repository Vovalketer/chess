#ifndef FEN_H
#define FEN_H
#include "../include/types.h"

typedef struct {
	char str[96];
} FenString;

bool	  fen_parse(const char* fen, Board* board);
FenString fen_from_board(const Board* board);

#endif
