#include <stdio.h>
#include "defs.h"
#include "colors.h"

// to know in order to undo a move:
// from-to
// captured piece
// // make
// push (position.irreversibleAspects);
// ply++;
// update (position, move)
// ...
// // unmake
// ply--;
// pop (position.irreversibleAspects);
// // position is restored from stack

// TODO: get rid of moveType and only use the 16-bit fromto
// should split this into reversible and irreversible aspects
// so that the reversible aspects can be used
// both in make and unmake

void makeMove(BOARD_STATE *bs, int from, int to, int moveType){
	// TODO: looking up the board is very expensive!!!
	// TODO: should I be using a pointer to a MOVE_STACK?
	// TODO: I think the logic for 3) is wonky...
	// I should be using nested if statements in a for loop
	// for(i = 1 ; i < 16 ; i = i * 2)
	// and maybe capture will only be set on ms
	// if moveType == 4

	// 1) update the move stack with info about current pos
	MOVE_STACK *ms = &(bs -> history[bs -> ply]);
	int capturedPiece = bs -> board[to];
	unsigned short int fromto = 0;
	fromto |= sq120to64(from) << 10;
	fromto |= sq120to64(to) << 4;
	fromto |= moveType;
	ms -> fromto = fromto;
	ms -> enPas = bs -> enPas;
	ms -> castlePermission = bs -> castlePermission;
	ms -> capturedPiece = capturedPiece;

	// 2) increment ply (index)
	bs -> ply++;

	// 3) update board with the move
	// ----------------------
	int *board = bs -> board;
	int piece = board[from];
	int cperm = bs -> castlePermission;

	// special stuff
	switch(moveType){
		case 0: break;
		case 1:
			// setting enPas
			if(isPawn[piece] && abs(to - from) == 20){
				bs -> enPas = to + (1 - 2 * getColor(piece)) * 10;
			}
			break;
		case 2:
		case 3:
			// TODO: put some of his in case 2:
			// TODO: assert during FEN parse that castleperm implies rook and king location
			// i.e. KQkq -> rooks in the corners and kings on the proper squares
			// castling, move the rook
			switch(to){
				case 97: board[98] = EMPTY; board[96] = wR; break;
				case 93: board[91] = EMPTY; board[94] = wR; break;
				case 27: board[28] = EMPTY; board[26] = bR; break;
				case 23: board[21] = EMPTY; board[24] = bR; break;
				default:
					printf(RED "Error castling: %d\n" reset, from);
					printf(RED "Error castling: %d\n" reset, board[from]);
					printf(RED "Error castling: %d\n" reset, bs -> castlePermission);
					exit(1);
			}
			break;
		case 4: break;
		case 5:
			// en passant
			// capture the enPas pawn
			board[to + (1 - 2 * getColor(piece)) * 10] = EMPTY;
			break;
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			// promotion
			// TODO: obviously finish this logic for all promo types
			if(isPawn[board[to]] && ((sq120to64(to)>=0&&sq120to64(to)<=7)||(sq120to64(to)>=56&&sq120to64(to)<=63))){
				if(getColor(board[to])){
					board[to] = bQ;
				} else {
					board[to] = wQ;
				}
			}
		break;
		default:
			printf(RED "Error with moveType on makeMove\n" reset);
			exit(1);
	}

	// TODO: these need to be elsewhere
	// Rook moves, cperm check at the end is only for efficiency
	if(piece == wR){
		if(from == 98 && cperm & WKCA){ bs -> castlePermission &= 7; }
		if(from == 91 && cperm & WQCA){ bs -> castlePermission &= 11; }
	}
	if(piece == bR){
		if(from == 28 && cperm & BKCA){ bs -> castlePermission &= 13; }
		if(from == 21 && cperm & BQCA){ bs -> castlePermission &= 14; }
	}
	// unset en passant sq
	if(moveType != 1){
		bs -> enPas = OFFBOARD;
	}
	// unset cperms on king moves, castling or not
	if(isKing[piece]){
		if(getColor(piece)){
			ASSERT(piece == bK);
			// TODO: put these numbers in terms of WKCA, etc.
			bs -> castlePermission &= 12;
		} else {
			ASSERT(piece == wK);
			bs -> castlePermission &= 3;
		}
	}

	// setting the pieces and switching side
	board[to] = board[from];
	board[from] = EMPTY;
	// is this the best way to switch sides?
	bs -> side = getColor(board[to]) ? WHITE : BLACK;
}

void undoMove(BOARD_STATE *bs){
	int from, to, moveType;

	// 1) decrement ply
	bs -> ply--;

	MOVE_STACK *ms = &(bs -> history[bs -> ply]);
	int capturedPiece = ms -> capturedPiece;
	unsigned short int fromto = ms -> fromto;
	from = sq64to120(fromto >> 10);
	to = sq64to120((fromto >> 4) & 63);
	moveType = fromto & 15;

	// 2) restore reversible
	int *board = bs -> board;
	// setting the pieces and switching side
	board[from] = board[to];
	board[to] = EMPTY;
	// is this the best way to switch sides?
	bs -> side = getColor(board[to]) ? WHITE : BLACK;

	// 3) restore irreversible
	// 3.1) restore non-board stuff
	bs -> enPas = ms -> enPas;
	bs -> castlePermission = ms -> castlePermission;
	capturedPiece = ms -> capturedPiece;

	// 3.2) restore board
	// regular captures
	if(moveType & 4 == 0){
		board[to] = EMPTY;
	}
	if(moveType == 4){
		board[to] = capturedPiece;
	}
	// en passant capture
	if(moveType == 5){
		board[to] = EMPTY;
		// TODO: I use getColor elsewhere
		board[to + (1 - 2 * !getColor(capturedPiece)) * 10] = capturedPiece;
	}
	// uncastling, move the rook
	if(moveType == 2 || moveType == 3){
		switch(to){
			case 97: board[98] = wR; board[96] = EMPTY; break;
			case 93: board[91] = wR; board[94] = EMPTY; break;
			case 27: board[28] = bR; board[26] = EMPTY; break;
			case 23: board[21] = bR; board[24] = EMPTY; break;
			default:
				printf(RED "Error uncastling: %d\n" reset, board[from]);
				exit(1);
		}
	}
}

