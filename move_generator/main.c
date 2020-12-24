#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>	
#include <time.h>
#include "defs.h"
#include "colors.h"


void resetBoard(BOARD_STATE *bs);
void printBoard(BOARD_STATE *bs, int options);
int pieceMoves(int *moves, BOARD_STATE *bs, int piece, int sq);
int genRandomMove(BOARD_STATE *bs);
int printAllMoves(int *moves, BOARD_STATE *bs, int side);
void testPieceMoves(int *moves, BOARD_STATE *bs, int piece, int sq);
void saveMove(int from, int to, int capture);
void makeMove(BOARD_STATE *bs, int from, int to);
int getType(int piece);
int getColor(int piece);
int isCheck(BOARD_STATE *bs, int color);
int newBoardCheck(int *board, int sq, int cs);
// TODO: somehow there's an issue with the global var
int mode;

int sq64to120(int sq64){
	return sq64 + 21 + 2 * (sq64 - sq64 % 8) / 8;
}

int sq120to64(int sq120){
	return sq120 - 17 - 2 * (sq120 - sq120 % 10) / 10;;
}

int frToSq64(int file, int rank){
	return (8 - rank) * 8 + file;
}

void sqName(char *sqfr, int sq120){
	int sq64 = sq120to64(sq120);
	char sqstr[] = {(sq64 % 8) + 'a', '8' - ((sq64 - sq64 % 8) / 8), '\0'};;
	if(sq120 == 0){ char *p = sqstr; *p++ = '-' ; *p++ = '\0'; }
	strcpy(sqfr, sqstr);
}

int getType(int piece){
	return (piece - 2) % 6;
}

// TODO: should I make an isOccupied()?
int getColor(int piece){
	// black is 7 - 12
	return piece > 6 && piece < 13;
}

void makeMove(BOARD_STATE *bs, int from, int to){
	bs -> board[to] = bs -> board[from];
	bs -> board[from] = EMPTY;

	// promotion
	// TODO: obviously move this somewhere else
	int sq = sq120to64(to);
	if(isPawn[bs -> board[to]] && ((sq>=0&&sq<=7)||(sq>=56&&sq<=63))){
		if(getColor(bs -> board[to])){
			bs -> board[to] = bQ;
		} else {
			bs -> board[to] = wQ;
		}
	}

	// castling
	if(
}

void saveMove(int from, int to, int capture){
	// TODO: remove this, temporary workaround
	// extern int[1000][3] legalMoves;
	for(int m  = 0 ; m < 1000 ; m++){
		if(legalMoves[m][2] == -1){
			legalMoves[m][0] = from;		
			legalMoves[m][1] = to;		
			legalMoves[m][2] = capture;		
			break;
		}
	}

	// char sqfr[2];
	// printf(YEL " == move: " reset);
	// sqName(sqfr, from);
	// printf("from: %s ", sqfr);
	// sqName(sqfr, to);
	// printf("to: %s ", sqfr);
	// printf("capture: %s\n", capture ? "yes" : "no");

	printf(CYN "mode: %d\n", mode);
}

int genRandomMove(BOARD_STATE *bs){
	// TODO: remove this, temporary workaround
	// extern int[1000][3] legalMoves;
	for(int m  = 0 ; m < 1000 ; m++){
		legalMoves[m][2] = -1;
	}

	int moves[27];

	int i, piece, sq, total = 0, side;
	for(i = 0 ; i < 64 ; i++){
		sq = sq64to120(i);
		piece = bs -> board[sq];
		side = bs -> side;
		if(piece != EMPTY && getColor(piece) == side){
			total += pieceMoves(moves, bs, piece, sq);
		}
	}

	char sqfrFrom[2];
	char sqfrTo[2];
	for(int m  = 0 ; m < total ; m++){
		sqName(sqfrFrom, legalMoves[m][0]);
		sqName(sqfrTo, legalMoves[m][1]);
		// printf(CYN "legalMoves[%d]: %s -> %s\n" reset, m, sqfrFrom, sqfrTo);
	}
	
	if(total > 0){
		srand(time(0));
		int r = rand() % total;
		makeMove(bs, legalMoves[r][0], legalMoves[r][1]);
		return r;
	} else {
		return -1;
	}
}

int printAllMoves(int *moves, BOARD_STATE *bs, int side){
	int i, piece, sq, total = 0;
	for(i = 0 ; i < 64 ; i++){
		sq = sq64to120(i);
		piece = bs -> board[sq];
		if(piece != EMPTY && getColor(piece) == side){
			printf(GRN " == PIECE: %c ==\n" reset, pieceChar[piece]);
			total += pieceMoves(moves, bs, piece, sq);
		}
	}
	printf(BLU "total moves in pos: %d\n" reset, total);
}

// given a color and board, is that side in check?
// TODO: wow why am I copy pasting my move gen logic
int isCheck(BOARD_STATE *bs, int color){
	int i, piece, cs, sq, moves[27];
	int *board = bs -> board;
	int numDirections[] = {8, 4, 4, 8, 8};
	int translation[][8] = {
		{-21, -12, 8, 19, 21, 12, -8, -19}, // knights 0
		{-11, 9, 11, -9}, // bishops 1
		{-10, -1, 10, 1}, // rooks 2
		{-11, 9, 11, -9, -10, -1, 10, 1}, // queens 3
		{-11, 9, 11, -9, -10, -1, 10, 1}  // kings 4
	};

	for(i = 0 ; i < 64 ; i++){
		sq = sq64to120(i);
		piece = bs -> board[sq64to120(i)];	
		if(piece != EMPTY && getColor(piece) != color){

			// pieces
			if(!isPawn[piece]){
				int d, type = getType(piece);
				for(d = 0 ; d < numDirections[type] ; d++){
					cs = sq;
					while(board[cs += translation[type][d]] != OFFBOARD){
						// if the piece is the king of opposite color
						// if not empty, either break or its the king
						if(board[cs] != EMPTY){
							if(getColor(board[cs]) == color && isKing[board[cs]]){
								return true;
							}
							else { break; }
						}
						if(type == 0 || type == 4){ break; }
					}
				}
			} else {
				// pawns
				cs = sq - (1 - 2 * getColor(piece)) * 10 + 1;
				if(board[cs] != OFFBOARD && getColor(board[cs]) == color && isKing[board[cs]]){
					return true;
				}
				cs = sq - (1 - 2 * getColor(piece)) * 10 - 1;
				if(board[cs] != OFFBOARD && getColor(board[cs]) == color && isKing[board[cs]]){
					return true;
				}
			}
		}
	}
	return false;
}

int newBoardCheck(int *board, int sq, int cs){
	// TODO: a bit clunky passing in a board and then passing a BOARD_STATE to isCheck()
	BOARD_STATE tbs[1];
	resetBoard(tbs);
	for(int i = 0 ; i < 120 ; i++){
		tbs -> board[i] = board[i];
	}
	tbs -> board[sq] = EMPTY;
	tbs -> board[cs] = board[sq];
	return isCheck(tbs, getColor(board[sq]));
}

int pieceMoves(int *moves, BOARD_STATE *bs, int piece, int sq){
	// plan here is to use the 120 sq board
	// and move in particular "directions"
	// until the piece is OFFBOARD
	// cs = candidate square
	// note that newBoardCheck() is called every time there is a new cs
	// This means that I check it prematurely in some cases
	// but it's cleaner for now
	// TODO: should I get color from the BOARD_STATE?
	int i = 0, cs = sq, enPasCaptureFromSq = OFFBOARD, total = 0;
	int *board = bs -> board;

	int numDirections[] = {8, 4, 4, 8, 8};
	int translation[][8] = {
		{-21, -12, 8, 19, 21, 12, -8, -19}, // knights 0
		{-11, 9, 11, -9}, // bishops 1
		{-10, -1, 10, 1}, // rooks 2
		{-11, 9, 11, -9, -10, -1, 10, 1}, // queens 3
		{-11, 9, 11, -9, -10, -1, 10, 1}  // kings 4
	};

	// initialize moves array
	for(i = 0 ; i < 27 ; i++){
		moves[i] = -1;
	}
	i = 0;
	// TODO: is there a better way to check for invalid index?
	ASSERT(sq >= 0 && sq <= 120 && board[sq] != OFFBOARD);

	// a better move generation
	if(!isPawn[piece]){
		// type = proper index for translation[][]
		int d, type = getType(piece);
		// for each direction
		for(d = 0 ; d < numDirections[type] ; d++){
			cs = sq;
			// while sq not offboard
			while(board[cs += translation[type][d]] != OFFBOARD){
				if(!newBoardCheck(board, sq, cs)){
					if(board[cs] == EMPTY){
						saveMove(sq, cs, 0);  moves[i] = cs; i++; total++;
					} else {
						if(getColor(piece) != getColor(board[cs])){ saveMove(sq, cs, 1); moves[i] = cs; i++; total++ ; }
						break;
					}
				}
				// stop if piece is a knight or king
				if(type == 0 || type == 4){ break; }
			}
		}
	} else {
		// pawns

		// forward 1
		// mapping {0,1} -> {-1,1} -> {-10,10}
		cs = sq - (1 - 2 * getColor(piece)) * 10;
		if(board[cs] == EMPTY && !newBoardCheck(board, sq, cs)){ saveMove(sq, cs, 0);  moves[i] = cs; i++; total++; }
		// forward 2
		if(sq - 80 + 50 * getColor(piece) > 0 && sq - 80 + 50 * getColor(piece) < 9){
			cs = sq - (1 - 2 * getColor(piece)) * 20;
			if(board[cs] == EMPTY && !newBoardCheck(board, sq, cs)){
				// TODO: set enPas, although that would probably be in a different function
				saveMove(sq, cs, 0);  moves[i] = cs; i++; total++;
			}
		}

		// captures
		cs = sq - (1 - 2 * getColor(piece)) * 10 + 1;
		if(board[cs] != EMPTY && board[cs] != OFFBOARD && getColor(piece) != getColor(board[cs]) && !newBoardCheck(board, sq, cs)){
			saveMove(sq, cs, 1);  moves[i] = cs; i++; total++;
		}
		cs = sq - (1 - 2 * getColor(piece)) * 10 - 1;
		if(board[cs] != EMPTY && board[cs] != OFFBOARD && getColor(piece) != getColor(board[cs]) && !newBoardCheck(board, sq, cs)){
			saveMove(sq, cs, 1);  moves[i] = cs; i++; total++;
		}
		// enPas
		cs = bs -> enPas;
		if(cs != EMPTY && !newBoardCheck(board, sq, cs)){
			ASSERT((cs <= 78 && cs >= 71) || (cs <= 48 && cs >= 41));
			// enPasCaptureFromSq is the same as what it would look like to captrue TO that sq
			enPasCaptureFromSq = cs - (1 - 2 * !getColor(piece)) * 10 + 1;
			if(sq == enPasCaptureFromSq){
				saveMove(sq, cs, 1);  moves[i] = cs; i++; total++;
			}
			enPasCaptureFromSq = cs - (1 - 2 * !getColor(piece)) * 10 - 1;
			if(sq == enPasCaptureFromSq){
				saveMove(sq, cs, 1);  moves[i] = cs; i++; total++;
			}
		}
	}
	// castling
	int cp = bs -> castlePermission;
	if(piece == wK){
		ASSERT(sq == 95);
		// if cp exists, not in check and not thru check and not thru piece
		if(cp & WKCA && !newBoardCheck(board, sq, sq) && !newBoardCheck(board, sq, sq+1) && !newBoardCheck(board, sq, sq+2)){
			saveMove(sq, sq + 2, 0);
		}
		if(cp & WQCA && !newBoardCheck(board, sq, sq) && !newBoardCheck(board, sq, sq-1) && !newBoardCheck(board, sq, sq-2)){
			saveMove(sq, sq - 2, 0);
		}
	}
	if(piece == wK){
		ASSERT(sq == 25);
		if(cp & BKCA && !newBoardCheck(board, sq, sq) && !newBoardCheck(board, sq, sq+1) && !newBoardCheck(board, sq, sq+2)){
			saveMove(sq, sq + 2, 0);
		}
		if(cp & BQCA && !newBoardCheck(board, sq, sq) && !newBoardCheck(board, sq, sq-1) && !newBoardCheck(board, sq, sq-2)){
			saveMove(sq, sq - 2, 0);
		}
	}
	return total;
}

void resetBoard(BOARD_STATE *bs){
	// this can go on a diet
	int i;
	for(i = 0 ; i < 120 ; i++){
		bs -> board[i] = OFFBOARD;
	}
	for(i = 0 ; i < 64 ; i++){
		bs -> board[sq64to120(i)] = EMPTY;
	}
	bs -> side = BOTH;
	bs -> castlePermission = 0;
	// TODO: make sure these are always 120sq
	// because EMPTY = 0
	bs -> enPas = EMPTY;
}

void printBoard(BOARD_STATE *bs, int option){
	int i, rank, file, piece, sq64;

	if(option == 1){
		printf(BLU "\n120 Board:\n\n" reset);
		for(i = 0 ; i < 120 ; i++){
			printf("%2d ", bs -> board[i]);
			if((i + 1) % 10 == 0){
				printf("\n");
			}
		}
		printf("\n");
	}

  printf(YEL "\nGame Board:\n\n" reset);
	for(rank = 8 ; rank > 0 ; rank--){
		printf("%d ", rank);
		for(file = 0 ; file < 8 ; file++){
			piece = bs -> board[sq64to120(frToSq64(file, rank))];
			printf("%2c", pieceChar[piece]);
		}
		printf("\n");
	}
	// print the files
	printf("\n  ");
	for(file = 0 ; file < 8 ; file++){
		printf(RED "%2c" reset, file + 'a');
	}
	printf("\n");

	printf(BLU "side to move: %s\n" reset, bs -> side == WHITE ? "white" : "black");
	char sqfr[2];
	sqName(sqfr, bs -> enPas);
	printf(BLU "enPas namesquare: %s\n" reset, sqfr);
	printf(BLU "enPas 120square: %d\n" reset, bs -> enPas);
	printf(BLU "WisCheck: %s\n" reset, isCheck(bs, WHITE) ? "true" : "false");
	printf(BLU "BisCheck: %s\n" reset, isCheck(bs, BLACK) ? "true" : "false");

}

void testPieceMoves(int *moves, BOARD_STATE *bs, int piece, int sq){
	int i;
	// create a dummy board to see candidate squares
	BOARD_STATE tmpBoard[1];
	resetBoard(tmpBoard);
	tmpBoard -> board[sq] = piece;

	pieceMoves(moves, tmpBoard, piece, sq);
	printf(YEL " == piece: %c == \n" reset, pieceChar[piece]);
	for(i = 0 ; moves[i] != -1 ; i++){
		tmpBoard -> board[moves[i]] = CANDIDATESQ;
	}

	printBoard(tmpBoard, 0);
}

int parseArgs(char *inputFEN, int argc, char *argv[]){
	int c;
	opterr = 0;
  while ((c = getopt (argc, argv, "f:")) != -1){
		switch (c){
			case 'f':
				strcpy(inputFEN, optarg);
				// TODO: this is clunky, need a better way to check this
				return 1;
				break;
			case '?':
				if (optopt == 'f')
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint(optopt))
					fprintf(stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				return -1;
			default:
				exit(1);
		}
	}
	// default to NORMAL_MODE
	return 0;
}

int main(int argc, char *argv[]){
	BOARD_STATE bs[1];
	int moves[27];
	// must always initialize board!
	resetBoard(bs);

	char inputFEN[99];
	char outputFEN[99];

	enum { NORMAL_MODE, FEN_MODE } mode = NORMAL_MODE;
	// for now, should only have these types
	ASSERT(argc == 3 || argc == 1);
	mode = parseArgs(inputFEN, argc, argv);
	switch(mode){
		case NORMAL_MODE:
			break;
		case FEN_MODE:
			break;
		default:
			printf(RED "ERROR: invalid mode: %d\n" reset, mode);
			break;
	}

	printf(YEL "mode: %d\n" reset, mode);
	// TODO: put this inside the switch block
	// NORMAL_MODE
	if(mode == 0){
		printf("Checking board initialization...\n");
		ASSERT(bs -> castlePermission == 0);
		char tmp[] = "8/PBk2r2/2R5/P5p1/P3NpPP/1b1PP3/6R1/2B2KN1 w KQkq -";
		// char tmp[] = "rnk5/4bq2/p1n4r/Pp1PppNP/1P6/B1PP4/R2K1P1R/1N1B4 b KQkq -";
		parseFEN(tmp, bs);
		printBoard(bs, 0);
		printAllMoves(moves, bs, BLACK);
	}
	// FEN_MODE
	if(mode == 1){
		parseFEN(inputFEN, bs);
		// printBoard(bs, 0);
		// genFEN(outputFEN, bs);
		// printf(CYN "inputFEN: %s\n" reset, inputFEN);
		// printf(CYN "outputFEN: %s\n" reset, outputFEN);
		int r = genRandomMove(bs);
		genFEN(outputFEN, bs);
		printf("%s\n", outputFEN);
	}
}
