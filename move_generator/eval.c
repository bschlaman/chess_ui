#include <stdio.h>
#include <string.h>
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

	// float mobilityWeight = 0.1;
	int mobilityWeight = 1;
	// TODO: not sure if memcpy and switching sides is right here
	int legalMovesBkp[1000][4];
	memcpy(legalMovesBkp, legalMoves, 1000);
	int numLegalMoves = genLegalMoves(bs);
	memcpy(legalMoves, legalMovesBkp, 1000);

	int *board = bs -> board;
	int kings = 0;
	int queens = 0;
	int rooks = 0;
	int bishops = 0;
	int knights = 0;
	int pawns = 0;
	int piece;
	int factor;
	for(int i = 0 ; i < 64 ; i++){
		piece = board[sq64to120(i)];
		factor = 1 - 2 * !(bs -> side ^ getColor(piece));
		if(piece == wK || piece == bK) kings += factor;
		if(piece == wQ || piece == bQ) queens += factor;
		if(piece == wR || piece == bR) rooks += factor;
		if(piece == wB || piece == bB) bishops += factor;
		if(piece == wN || piece == bN) knights += factor;
		if(piece == wP || piece == bP) pawns += factor;
	}
	materialEval += kings * kingWeight;
	materialEval += queens * queenWeight;
	materialEval += rooks * rookWeight;
	materialEval += bishops * bishopWeight;
	materialEval += knights * knightWeight;
	materialEval += pawns * pawnWeight;
	mobilityEval = numLegalMoves * mobilityWeight;
	// avoid checkmate
	// TODO: make this better obviously
	if(numLegalMoves == 0){
		return -100000000;
	}
	return materialEval + mobilityEval;
}

int randInt(int lb, int ub){
	return rand() % (ub - lb + 1) + lb;
}

int negaMax(BOARD_STATE *bs, int depth){
	if(depth == 0) return eval(bs);
	int max = -100000, score;

	genLegalMoves(bs);
	int cpy[1000][4];
	for(int m = 0 ; m < 1000 ; m++){
		cpy[m][2] = -1;
	}
	// memcpy(cpy, legalMoves, numMoves * sizeof(cpy[0]));
	memcpy(cpy, legalMoves, 1000 * sizeof(cpy[0]));

	int i = 0;
	while(cpy[i][2] != -1){
		makeMove(bs, legalMoves[i][0], legalMoves[i][1], legalMoves[i][2]);
		score = -1 * negaMax(bs, depth - 1);
		undoMove(bs);
		if(score > max) max = score;
		i++;
	}
	return max;
}

unsigned short int getNextMove(){
	return NULL;
}

int treeSearch(BOARD_STATE *bs, int depth){
	int m, from, to, moveType;
	int posEval;
	char sqfr[3];
	genLegalMoves(bs);
	int bestScore = -100000;
	for(m = 0 ; legalMoves[m][2] != -1 ; m++){
		from = legalMoves[m][0];
		to = legalMoves[m][1];
		moveType = legalMoves[m][2];

		makeMove(bs, from, to, moveType);
		posEval = -1 * eval(bs);
		undoMove(bs);
		legalMoves[m][3] = posEval;
		if(posEval > bestScore) bestScore = posEval;
	}
	return bestScore;
}






















