#include <stdio.h>
#include "defs.h"
#include "colors.h"

int eval(BOARD_STATE *bs){
	int materialEval = 0;
	int mobilityEval = 0;

	int kingWeight = 200;
	int queenWeight = 9;
	int rookWeight = 5;
	int bishopWeight = 3;
	int knightWeight = 3;
	int pawnWeight = 1;

	float mobilityWeight = 0.1;

	int *board = bs -> board;
	int kings;
	int queens;
	int rooks;
	int bishops;
	int knights;
	int pawns;
	int piece;
	for(int i = 0 ; i < 64 ; i++){
		piece = board[i];
		if(piece == wK) pawns++;
		if(piece == wQ) pawns++;
		if(piece == wR) pawns++;
		if(piece == wN) pawns++;
			
	}
}
