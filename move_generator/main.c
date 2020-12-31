#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>	
#include "defs.h"
#include "colors.h"


void printBoard(BOARD_STATE *bs, int options);
int pieceMoves(BOARD_STATE *bs, int piece, int sq);
int genRandomMove(BOARD_STATE *bs);
void printAllMoves(BOARD_STATE *bs);
void saveMove(int from, int to, int moveType);
int isCheck(BOARD_STATE *bs, int color);
int newBoardCheck(int *board, int sq, int cs);
// TODO: somehow there's an issue with the global var

int sq64to120(int sq64){
	return sq64 + 21 + 2 * (sq64 - sq64 % 8) / 8;
}

int sq120to64(int sq120){
	return sq120 - 17 - 2 * (sq120 - sq120 % 10) / 10;;
}

int frToSq64(int file, int rank){
	return (8 - rank) * 8 + file;
}

void getAlgebraic(char *sqStrPtr, int sq120){
	int sq64 = sq120to64(sq120);
	char sqAN[] = {(sq64 % 8) + 'a', '8' - ((sq64 - sq64 % 8) / 8), '\0'};;
	if(sq120 == OFFBOARD){ char *p = sqAN; *p++ = '-' ; *p++ = '\0'; }
	strcpy(sqStrPtr, sqAN);
}

void getCastlePermissions(char *sqStrPtr, int cperm){
	int j = 0;
	if(cperm == 0){
		sqStrPtr[j] = '-';
		sqStrPtr[j+1] = '\0';
	} else {
		for(int i = 0 ; i < 4 ; i++){
			if(1 << (3 - i) & cperm){
				sqStrPtr[j] = castleChar[i];
				j++;
			}
		}
		sqStrPtr[j] = '\0';
	}
}

int getType(int piece){
	return (piece - 2) % 6;
}

// TODO: should I make an isOccupied()?
int getColor(int piece){
	// black is 7, 8, 9, 10, 11, 12
	return piece > 6 && piece < 13;
}

void saveMove(int from, int to, int moveType){
	// TODO: remove this, temporary workaround
	int m;
	for(m = 0 ; m < 1000 ; m++){
		if(legalMoves[m][2] == -1){
			legalMoves[m][0] = from;
			legalMoves[m][1] = to;
			legalMoves[m][2] = moveType;
			break;
		}
	}
}

int genLegalMoves(BOARD_STATE *bs){
	int i, total = 0;
	int sq, piece, side;
	side = bs -> side;

	initLegalMoves();
	for(i = 0 ; i < 64 ; i++){
		sq = sq64to120(i);
		piece = bs -> board[sq];
		if(piece != EMPTY && getColor(piece) == side){
			total += pieceMoves(bs, piece, sq);
		}
	}
	return total;
}

int genRandomMove(BOARD_STATE *bs){
	int total = genLegalMoves(bs);
	if(total > 0){
		int r = rand() % total;
		return r;
	} else {
		// TODO: stalemate
		return -1;
	}
}

// BOARD_STATE arg needed for eval
void printLegalMoves(BOARD_STATE *bs){
	int m, from, to, moveType;
	char sqfr[3];
	for(m = 0 ; legalMoves[m][2] != -1 ; m++){
		from = legalMoves[m][0];
		to = legalMoves[m][1];
		moveType = legalMoves[m][2];

		printf(YEL "      move %d: " reset, m);
		getAlgebraic(sqfr, from);
		printf("from: %s ", sqfr);
		getAlgebraic(sqfr, to);
		printf("to: %s ", sqfr);
		printf("moveType: %d ", moveType);
		// evaluate position after move is made
		makeMove(bs, from, to, moveType);
		printf("eval for opponent after move: %d\n", eval(bs));
		undoMove(bs);
	}
}

void printAllMoves(BOARD_STATE *bs){
	int i, m, piece, sq, total = 0;
	int side = bs -> side;
	int from, to, moveType;

	for(i = 0 ; i < 64 ; i++){
		initLegalMoves();
		sq = sq64to120(i);
		piece = bs -> board[sq];
		if(piece != EMPTY && getColor(piece) == side){
			printf(GRN " == PIECE: %c\n" reset, pieceChar[piece]);
			total += pieceMoves(bs, piece, sq);
			printLegalMoves(bs);
		}
	}
	printf(BLU "total moves in pos: " reset "%d\n", total);
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

int enPasCorrectColor(int enPas, int side){
	return enPas - 40 - 30 * side >= 1 && enPas - 40 - 30 * side <= 8;
}

int newBoardCheck(int *board, int sq, int cs){
	// TODO: a bit clunky passing in a board and then passing a BOARD_STATE to isCheck()
	// hypothetical board state
	BOARD_STATE *hbs = malloc(sizeof(BOARD_STATE));;
	resetBoard(hbs);
	for(int i = 0 ; i < 120 ; i++){
		hbs -> board[i] = board[i];
	}
	hbs -> board[sq] = EMPTY;
	hbs -> board[cs] = board[sq];
	return isCheck(hbs, getColor(board[sq]));
}

int pieceMoves(BOARD_STATE *bs, int piece, int sq){
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

	ASSERT(sq >= 0 && sq <= 120 && board[sq] != OFFBOARD);

	// move generation
	if(!isPawn[piece]){
		// type = proper index for translation[][]
		int d, type = getType(piece);
		// for each direction
		for(d = 0 ; d < numDirections[type] ; d++){
			cs = sq;
			// while sq not offboard
			while(board[cs += translation[type][d]] != OFFBOARD){
				if(true){
					if(board[cs] == EMPTY && !newBoardCheck(board, sq, cs)){
						saveMove(sq, cs, 0); total++;
					} else {
						if(getColor(piece) != getColor(board[cs]) && !newBoardCheck(board, sq, cs)){ saveMove(sq, cs, 4); total++; }
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
		if(board[cs] == EMPTY && !newBoardCheck(board, sq, cs)){ saveMove(sq, cs, 0); total++; }
		// forward 2
		if(sq - 80 + 50 * getColor(piece) > 0 && sq - 80 + 50 * getColor(piece) < 9){
			cs = sq - (1 - 2 * getColor(piece)) * 20;
			if(board[cs] == EMPTY && !newBoardCheck(board, sq, cs)){
				saveMove(sq, cs, 1); total++;
			}
		}

		// captures
		cs = sq - (1 - 2 * getColor(piece)) * 10 + 1;
		if(board[cs] != EMPTY && board[cs] != OFFBOARD && getColor(piece) != getColor(board[cs]) && !newBoardCheck(board, sq, cs)){
			saveMove(sq, cs, 4); total++;
		}
		cs = sq - (1 - 2 * getColor(piece)) * 10 - 1;
		if(board[cs] != EMPTY && board[cs] != OFFBOARD && getColor(piece) != getColor(board[cs]) && !newBoardCheck(board, sq, cs)){
			saveMove(sq, cs, 4); total++;
		}
		// enPas
		cs = bs -> enPas;
		if(cs != OFFBOARD && !newBoardCheck(board, sq, cs)){
			// ASSERT((cs <= 78 && cs >= 71) || (cs <= 48 && cs >= 41));
			// printf(CYN "assrt: %d\n" reset, cs - 40 - 30 * getColor(piece));
			// ASSERT(cs - 40 - 30 * getColor(piece) >= 1 && cs - 40 - 30 * getColor(piece) <= 8);
			// enPasCaptureFromSq is the same as what it would look like to captrue TO that sq
			enPasCaptureFromSq = cs - (1 - 2 * !getColor(piece)) * 10 + 1;
			if(sq == enPasCaptureFromSq && enPasCorrectColor(cs, getColor(piece))){
				saveMove(sq, cs, 5); total++;
			}
			enPasCaptureFromSq = cs - (1 - 2 * !getColor(piece)) * 10 - 1;
			if(sq == enPasCaptureFromSq && enPasCorrectColor(cs, getColor(piece))){
				saveMove(sq, cs, 5); total++;
			}
		}
	}

	// castling
	int cperm = bs -> castlePermission;
	if(piece == wK){
		// if cperm exists, not in check and not thru check and not thru piece
		if(cperm & WKCA \
			&& !newBoardCheck(board, sq, sq) \
			&& !newBoardCheck(board, sq, sq + 1) \
			&& !newBoardCheck(board, sq, sq + 2) \
			&& board[96] == EMPTY && board[97] == EMPTY){
			ASSERT(sq == 95);
			saveMove(sq, sq + 2, 2); total++;
		}
		if(cperm & WQCA \
			&& !newBoardCheck(board, sq, sq) \
			&& !newBoardCheck(board, sq, sq - 1) \
			&& !newBoardCheck(board, sq, sq - 2) \
			&& board[92] == EMPTY && board[93] == EMPTY && board[94] == EMPTY){
			ASSERT(sq == 95);
			saveMove(sq, sq - 2, 3); total++;
		}
	}
	if(piece == bK){
		if(cperm & BKCA \
			&& !newBoardCheck(board, sq, sq) \
			&& !newBoardCheck(board, sq, sq + 1) \
			&& !newBoardCheck(board, sq, sq + 2) \
			&& board[26] == EMPTY && board[27] == EMPTY){
			ASSERT(sq == 25);
			saveMove(sq, sq + 2, 2); total++;
		}
		if(cperm & BQCA \
			&& !newBoardCheck(board, sq, sq) \
			&& !newBoardCheck(board, sq, sq - 1) \
			&& !newBoardCheck(board, sq, sq - 2) \
			&& board[22] == EMPTY && board[23] == EMPTY && board[24] == EMPTY){
			ASSERT(sq == 25);
			saveMove(sq, sq - 2, 3); total++;
		}
	}
	return total;
}

void resetBoard(BOARD_STATE *bs){
	int i;
	for(i = 0 ; i < 120 ; i++){
		bs -> board[i] = OFFBOARD;
	}
	for(i = 0 ; i < 64 ; i++){
		bs -> board[sq64to120(i)] = EMPTY;
	}
	// I should make these the starting pos
	// and load the startFEN
	bs -> side = NEITHER;
	bs -> castlePermission = 0;
	bs -> enPas = OFFBOARD;
	bs -> ply = 1;
}

void printBoard(BOARD_STATE *bs, int option){
	int i, rank, file, piece, sq64;

	if(option == OPT_120_BOARD){
		printf(YEL " ---- 120 Board ---- \n" reset);
		for(i = 0 ; i < 120 ; i++){
			printf("%2d ", bs -> board[i]);
			if((i + 1) % 10 == 0){
				printf("\n");
			}
		}
		printf("\n");
	}

	if(option == OPT_64_BOARD){
		printf(YEL " ---- Game Board ---- \n" reset);
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
	}

	if(option == OPT_BOARD_STATE){
		printf(BLU "side to move: " reset "%s\n", bs -> side == WHITE ? "white" : "black");
		printf(BLU "ply: " reset "%d\n", bs -> ply);
		char sqAN[3];
		char cperms[5];
		getAlgebraic(sqAN, bs -> enPas);
		getCastlePermissions(cperms, bs -> castlePermission);
		printf(BLU "en passant sq: " reset "%s\n", sqAN);
		printf(BLU "castlePerms: " reset "%s\n", cperms);
		printf(BLU "white in check: " reset "%s\n", isCheck(bs, WHITE) ? "true" : "false");
		printf(BLU "black in check: " reset "%s\n", isCheck(bs, BLACK) ? "true" : "false");
		printf(BLU "eval: " reset "%d\n", eval(bs));
		bs -> side = !(bs -> side);
		// TODO: the num legal moves might be innacurate
		// if just switching sides due to en passant?
		printf(BLU "eval for opponent: " reset "%d\n", eval(bs));
		bs -> side = !(bs -> side);
	}
}

int parseArgs(char *inputFEN, int argc, char *argv[]){
	int c;
  while((c = getopt(argc, argv, "srf:")) != -1){
		switch(c){
			case 'f':
				strcpy(inputFEN, optarg);
				return FEN_MODE;
				break;
			case 'n':
				return RAND_MODE;
				break;
			case 's':
				return SEARCH_MODE;
				break;
			case '?':
				if(optopt == 'f')
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if(isprint(optopt))
					fprintf(stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				return -1;
			default:
				fprintf(stderr, "Error with arg parsing", optopt);
				exit(1);
		}
	}
	// default to NORMAL_MODE
	return NORMAL_MODE;
}

int main(int argc, char *argv[]){
	initRand();
	BOARD_STATE *bs = initGame();

	// declaring here so that parseArgs can strcpy into inputFEN
	char inputFEN[99];
	char outputFEN[99];

	mode = parseArgs(inputFEN, argc, argv);
	switch(mode){
		case NORMAL_MODE:
			break;
		case FEN_MODE:
			break;
		case RAND_MODE:
			break;
		case SEARCH_MODE:
			break;
		default:
			printf(RED "ERROR: invalid mode: %d\n" reset, mode);
			exit(0);
	}

	// TODO: put this inside the switch block
	// NORMAL_MODE - print out all legal moves of a pos
	if(mode == NORMAL_MODE){
		printf("Checking board initialization...\n\n");
		ASSERT(bs -> castlePermission == 0 && bs -> enPas == OFFBOARD);
		// char testFEN[] = "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 w - - 0 1"; // max legalMoves
		// char testFEN[] = "4qr1k/6p1/4p2p/p2p2b1/1p2P1Q1/1PrB3P/P2R1PP1/3R2K1 w - -"; // kasparov|karpov
		char testFEN[] = "4qr1k/6p1/4p2p/p2pP3/1p6/1PrB3P/P2R1PPb/3R2K1 w - -"; // debug
		parseFEN(testFEN, bs);
		printBoard(bs, OPT_64_BOARD);
		printBoard(bs, OPT_BOARD_STATE);
		printAllMoves(bs);
		genFEN(outputFEN, bs);
		printf("%s\n", outputFEN);
	}
	// FEN_MODE
	else if(mode == FEN_MODE){
		parseFEN(inputFEN, bs);
		int r = genRandomMove(bs);
		makeMove(bs, legalMoves[r][0], legalMoves[r][1], legalMoves[r][2]);
		genFEN(outputFEN, bs);
		printf("%s\n", outputFEN);
	}
	else if (mode == RAND_MODE){
		printf("Checking board initialization...\n\n");
		ASSERT(bs -> castlePermission == 0 && bs -> enPas == OFFBOARD);
		char testFEN[] = "rnbqkbnr/pppppppp/8/8/6P1/8/PPPPPP1P/RNBQKBNR w KQkq g3";
		parseFEN(testFEN, bs);

		int r;
		for(int i = 0 ; i < 175 ; i++){
			r = genRandomMove(bs);
			if(r == -1){
				int side = bs -> side;
				printf(RED " @@@@@@@@@@@@ %s WINS!\n\n\n\n\n" reset, side ? "WHITE" : "BLACK");
				printBoard(bs, OPT_64_BOARD);
				printBoard(bs, OPT_BOARD_STATE);
				genFEN(outputFEN, bs);
				printf("%s\n", outputFEN);
				exit(0);
			}
			makeMove(bs, legalMoves[r][0], legalMoves[r][1], legalMoves[r][2]);
			if(i == 173){
				printBoard(bs, OPT_64_BOARD);
				printBoard(bs, OPT_BOARD_STATE);
			}
		}
		for(int j = 0 ; j < 175 ; j++){
			undoMove(bs);
		}
		printBoard(bs, OPT_64_BOARD);
		printBoard(bs, OPT_BOARD_STATE);
	}
	else if(mode == SEARCH_MODE){
		printf("Checking board initialization...\n\n");
		ASSERT(bs -> castlePermission == 0 && bs -> enPas == OFFBOARD);
		// char testFEN[] = "rnb1kbnr/ppp1p1pp/8/3p1p2/1q3P1B/2P1P3/PP1P2PP/RNBQK1NR w KQkq -";
		char testFEN[] = "4qr1k/6p1/4p2p/p2p2b1/1p2P1Q1/1PrB3P/P2R1PP1/3R2K1 w - -"; // kasparov|karpov
		parseFEN(testFEN, bs);
		while(true){
			getchar();
			printBoard(bs, OPT_64_BOARD);
			printBoard(bs, OPT_BOARD_STATE);
			int total = genLegalMoves(bs);
			printLegalMoves(bs);
			printf(BLU "total moves in pos: " reset "%d\n", total);
			int best = treeSearch(bs, 1);
			printf(BLU "best score: " reset "%d\n", best);
			getchar();
			for(int m = 0 ; legalMoves[m][2] != -1 ; m++){
				if(legalMoves[m][3] == best){
					printf(CYN "Making move: %d\n", m);
					makeMove(bs, legalMoves[m][0], legalMoves[m][1], legalMoves[m][2]);
					break;
				}
			}
		}
	}
}
