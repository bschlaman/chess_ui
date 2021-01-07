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
int isCheck(int *board, int kingsq, int color);

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

// TODO: this is risky for EMPTY squares
int getColor(int piece){
	// black is 7, 8, 9, 10, 11, 12
	return piece > 6 && piece < 13;
}

void saveMove(int from, int to, int moveType){
	// TODO: remove this, temporary workaround
	// TODO: what if I pass total as the index?
	int m;
	for(m = 0 ; m < 218 ; m++){
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

void printMove(int m, int from, int to, int moveType){
	char sqfr[3];
	printf(YEL "      move %d: " reset, m);
	getAlgebraic(sqfr, from);
	printf("from: %s ", sqfr);
	getAlgebraic(sqfr, to);
	printf("to: %s ", sqfr);
	printf("moveType: %d\n", moveType);
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

int isCheck(int *board, int kingsq, int color){
counter++;
	int d, cpiece, cs;

	// kings
	for(d = 0 ; d < 8 ; d++){
		// TODO: don't need to check if this is OFFBOARD
		// but may run into a -1 index if using isKing
		if((cpiece = board[kingsq + translation[KING][d]]) != OFFBOARD){
			// TODO: Assert the color?
			if(isKing[cpiece]) return true;
		}
	}
	// knights
	for(d = 0 ; d < 8 ; d++){
		if((cpiece = board[kingsq + translation[0][d]]) != OFFBOARD \
			&& getType(cpiece) == KNIGHT \
			&& color != getColor(cpiece)){
			return true;
		}
	}
	// pawns
	if(color == BLACK && board[kingsq + 9] == wP) return true;
	if(color == BLACK && board[kingsq + 11] == wP) return true;
	if(color == WHITE && board[kingsq - 9] == bP) return true;
	if(color == WHITE && board[kingsq - 11] == bP) return true;
	// ray
	for(d = 0 ; d < 8 ; d++){
		cs = kingsq;
		while((cpiece = board[cs += translation[KING][d]]) != OFFBOARD){
			if(cpiece != EMPTY){
				if(getColor(cpiece) != color){
					if(d < 4){
						if(getType(cpiece) == BISHOP || getType(cpiece) == QUEEN) return true;
					}
					if(d >= 4 && d < 8){
						if(getType(cpiece) == ROOK || getType(cpiece) == QUEEN) return true;
					}
				}
				break;
			}
		}
	}
	return false;
}
// given a color and board, is that side in check?
// TODO: wow why am I copy pasting my move gen logic
// TODO: this is the biggest slowdown in the program
// likely since I am looking up the entire board again
int oldisCheck(int *board, int color){
counter++;
	int i, piece, cs, sq, d, type;
	int cpiece;

	for(i = 0 ; i < 64 ; i++){
		sq = sq64to120(i);
		piece = board[sq64to120(i)];	
		if(piece != EMPTY && getColor(piece) != color){
			// pieces
			if(!isPawn[piece]){
				type = getType(piece);
				for(d = 0 ; d < numDirections[type] ; d++){
					cs = sq;
					while(cpiece = board[cs += translation[type][d]] != OFFBOARD){
						// if the piece is the king of opposite color
						// if not empty, either break or its the king
						if(cpiece != EMPTY){
							if(getColor(cpiece) == color && isKing[cpiece]){
								return true;
							}
							else { break; }
						}
						if(type == KNIGHT || type == KING){ break; }
					}
				}
			} else {
				// pawns
				cpiece = board[cs = sq - (1 - 2 * getColor(piece)) * 10 + 1];
				if(cpiece != OFFBOARD && getColor(cpiece) == color && isKing[cpiece]){
					return true;
				}
				cpiece = board[cs = sq - (1 - 2 * getColor(piece)) * 10 - 1];
				if(cpiece != OFFBOARD && getColor(cpiece) == color && isKing[cpiece]){
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

int newBoardCheckEP(BOARD_STATE *bs, int sq, int cs){
	int *board = bs -> board;
	int check = false;
	int side = bs -> side;
	int kingsq = bs -> kingSq[side];

	board[cs] = board[sq];
	board[sq] = EMPTY;
	board[cs + (1 - 2 * side) * 10] = EMPTY;
	check = isCheck(board, kingsq, side);
	board[sq] = board[cs];
	board[cs] = EMPTY;
	board[cs + (1 - 2 * side) * 10] = side ? wP : bP;
	return check;
}
int newBoardCheck(BOARD_STATE *bs, int sq, int cs){
	int *board = bs -> board;
	int check = false;
	int capturedPiece = board[cs];
	int side = bs -> side;
	int kingsq = bs -> kingSq[side];

	board[cs] = board[sq];
	board[sq] = EMPTY;
	if(kingsq == sq) kingsq = cs;
	check = isCheck(board, kingsq, side);
	board[sq] = board[cs];
	board[cs] = capturedPiece;
	return check;
}

int pieceMoves(BOARD_STATE *bs, int piece, int sq){
	// plan here is to use the 120 sq board
	// and move in particular "directions"
	// until the piece is OFFBOARD
	// cs = candidate square
	// cs2 = candidate square 2 (used for double pawn push)
	// note that newBoardCheck() is called every time there is a new cs
	// This means that I check it prematurely in some cases
	// but it's cleaner for now
	// TODO: should I get color from the BOARD_STATE?
	int i = 0, cs = sq, cs2, enPasCaptureFromSq = OFFBOARD, total = 0, d, type, cpiece;
	int *board = bs -> board;

	ASSERT(sq >= 0 && sq <= 120 && board[sq] != OFFBOARD);

	// move generation
	if(!isPawn[piece]){
		// type = proper index for translation[][]
		type = getType(piece);
		// for each direction
		for(d = 0 ; d < numDirections[type] ; d++){
			cs = sq;
			// while sq not offboard
			while((cpiece = board[cs += translation[type][d]]) != OFFBOARD){
				//if(!newBoardCheck(bs, sq, cs)){
				if(cpiece == EMPTY){
					if(!newBoardCheck(bs, sq, cs)){ saveMove(sq, cs, 0); total++; }
				} else {
					if(getColor(piece) != getColor(cpiece)){
						if(!newBoardCheck(bs, sq, cs)){ saveMove(sq, cs, 4); total++; }
					}
					break;
				}
				// stop if piece is a knight or king
				if(type == KNIGHT || type == KING){ break; }
			}
		}
	} else {
		// pawns =================

		// forward 1
		// mapping {0,1} -> {-1,1} -> {-10,10}
		cs = sq - (1 - 2 * getColor(piece)) * 10;
		if(board[cs] == EMPTY && !newBoardCheck(bs, sq, cs)){
			if(cs - 20 - 70 * getColor(piece) > 0 && cs - 20 - 70 * getColor(piece) < 9){
				saveMove(sq, cs, 8); total++;
				saveMove(sq, cs, 9); total++;
				saveMove(sq, cs, 10); total++;
				saveMove(sq, cs, 11); total++;
			} else {
				saveMove(sq, cs, 0); total++;
			}
		}
		// forward 2
		if(sq - 80 + 50 * getColor(piece) > 0 && sq - 80 + 50 * getColor(piece) < 9){
			cs = sq - (1 - 2 * getColor(piece)) * 20;
			cs2 = sq - (1 - 2 * getColor(piece)) * 10;
			if(board[cs] == EMPTY && board[cs2] == EMPTY && !newBoardCheck(bs, sq, cs)){
				saveMove(sq, cs, 1); total++;
			}
		}
		// captures
		cs = sq - (1 - 2 * getColor(piece)) * 10 + 1;
		if(board[cs] != EMPTY && board[cs] != OFFBOARD \
			&& getColor(piece) != getColor(board[cs]) \
			&& !newBoardCheck(bs, sq, cs)){
			if(cs - 20 - 70 * getColor(piece) > 0 && cs - 20 - 70 * getColor(piece) < 9){
				saveMove(sq, cs, 12); total++;
				saveMove(sq, cs, 13); total++;
				saveMove(sq, cs, 14); total++;
				saveMove(sq, cs, 15); total++;
			} else {
				saveMove(sq, cs, 4); total++;
			}
		}
		cs = sq - (1 - 2 * getColor(piece)) * 10 - 1;
		if(board[cs] != EMPTY && board[cs] != OFFBOARD \
			&& getColor(piece) != getColor(board[cs]) \
			&& !newBoardCheck(bs, sq, cs)){
			if(cs - 20 - 70 * getColor(piece) > 0 && cs - 20 - 70 * getColor(piece) < 9){
				saveMove(sq, cs, 12); total++;
				saveMove(sq, cs, 13); total++;
				saveMove(sq, cs, 14); total++;
				saveMove(sq, cs, 15); total++;
			} else {
				saveMove(sq, cs, 4); total++;
			}
		}
		// enPas
		cs = bs -> enPas;
		if(cs != OFFBOARD){
			// enPasCaptureFromSq is the same as what it would look like to captrue TO that sq
			enPasCaptureFromSq = cs - (1 - 2 * !getColor(piece)) * 10 + 1;
			if(sq == enPasCaptureFromSq \
				&& enPasCorrectColor(cs, getColor(piece)) \
				&& !newBoardCheckEP(bs, sq, cs)){
				saveMove(sq, cs, 5); total++;
			}
			enPasCaptureFromSq = cs - (1 - 2 * !getColor(piece)) * 10 - 1;
			if(sq == enPasCaptureFromSq \
				&& enPasCorrectColor(cs, getColor(piece)) \
				&& !newBoardCheckEP(bs, sq, cs)){
				saveMove(sq, cs, 5); total++;
			}
		}
	}

	// castling
	int cperm = bs -> castlePermission;
	if(piece == wK){
		// if cperm exists, not in check and not thru check and not thru piece
		if(cperm & WKCA \
			&& !newBoardCheck(bs, sq, sq) \
			&& !newBoardCheck(bs, sq, sq + 1) \
			&& !newBoardCheck(bs, sq, sq + 2) \
			&& board[96] == EMPTY && board[97] == EMPTY){
			ASSERT(sq == 95);
			saveMove(sq, sq + 2, 2); total++;
		}
		if(cperm & WQCA \
			&& !newBoardCheck(bs, sq, sq) \
			&& !newBoardCheck(bs, sq, sq - 1) \
			&& !newBoardCheck(bs, sq, sq - 2) \
			&& board[92] == EMPTY && board[93] == EMPTY && board[94] == EMPTY){
			ASSERT(sq == 95);
			saveMove(sq, sq - 2, 3); total++;
		}
	}
	if(piece == bK){
		if(cperm & BKCA \
			&& !newBoardCheck(bs, sq, sq) \
			&& !newBoardCheck(bs, sq, sq + 1) \
			&& !newBoardCheck(bs, sq, sq + 2) \
			&& board[26] == EMPTY && board[27] == EMPTY){
			ASSERT(sq == 25);
			saveMove(sq, sq + 2, 2); total++;
		}
		if(cperm & BQCA \
			&& !newBoardCheck(bs, sq, sq) \
			&& !newBoardCheck(bs, sq, sq - 1) \
			&& !newBoardCheck(bs, sq, sq - 2) \
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
	bs -> kingSq[WHITE] = 95;
	bs -> kingSq[BLACK] = 25;
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
		char sqAN[3];
		char cperms[5];
		char boardFEN[99];
		printf(BLU "side to move: " reset "%s\n", bs -> side == WHITE ? "white" : "black");
		printf(BLU "ply: " reset "%d\n", bs -> ply);
		getAlgebraic(sqAN, bs -> enPas);
		getCastlePermissions(cperms, bs -> castlePermission);
		printf(BLU "en passant sq: " reset "%s\n", sqAN);
		printf(BLU "castlePerms: " reset "%s\n", cperms);
		printf(BLU "white in check: " reset "%s\n", isCheck(bs -> board, bs -> kingSq[WHITE], WHITE) ? "true" : "false");
		printf(BLU "black in check: " reset "%s\n", isCheck(bs -> board, bs -> kingSq[BLACK], BLACK) ? "true" : "false");
		printf(BLU "white kingSq: " reset "%d\n", bs -> kingSq[WHITE]);
		printf(BLU "black kingSq: " reset "%d\n", bs -> kingSq[BLACK]);
		printf(BLU "eval: " reset "%d\n", eval(bs));
		bs -> side = !(bs -> side);
		// TODO: the num legal moves might be innacurate
		// if just switching sides due to en passant?
		// "eval for opponent" doesn't really make sense since it's not their turn
		printf(BLU "eval for opponent: " reset "%d\n", eval(bs));
		bs -> side = !(bs -> side);
		genFEN(boardFEN, bs);
		printf(BLU "fen: " reset "%s\n", boardFEN);
	}
}

int parseArgs(char *inputFEN, int argc, char *argv[]){
	int c;
  while((c = getopt(argc, argv, "spf:")) != -1){
		switch(c){
			case 'f':
				strcpy(inputFEN, optarg);
				return FEN_MODE;
				break;
			case 'p':
				return PERFT_MODE;
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
				fprintf(stderr, "Error with arg parsing\n", optopt);
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

	// NORMAL_MODE - print out all legal moves of a pos
	if(mode == NORMAL_MODE){
		printf("Checking board initialization...\n");
		ASSERT(bs -> castlePermission == 0 && bs -> enPas == OFFBOARD);
		printf("Checking movegen test...\n\n");
		ASSERT(testMoves());
		// char testFEN[] = "4qr1k/6p1/4p2p/p2p2b1/1p2P1Q1/1PrB3P/P2R1PP1/3R2K1 w - -"; // kasparov|karpov
		char testFEN[] = "rnbqkbnr/pppp3p/8/5Pp1/8/8/PPPPP1PP/RN2K3 w KQkq g6"; // debug
		parseFEN(testFEN, bs);
		printBoard(bs, OPT_64_BOARD);
		printBoard(bs, OPT_BOARD_STATE);

		makeMove(bs, 56, 47, 5);
		printBoard(bs, OPT_64_BOARD);
		printBoard(bs, OPT_BOARD_STATE);
		undoMove(bs);
		printBoard(bs, OPT_64_BOARD);
		printBoard(bs, OPT_BOARD_STATE);
	}

	// FEN_MODE - return fen of best move
	else if(mode == FEN_MODE){
		int myMoves[218][4];
		parseFEN(inputFEN, bs);
		int total = genLegalMoves(bs);
		memcpy(myMoves, legalMoves, 218 * sizeof(myMoves[0]));
		int best = -11111;
		int b = -1;
		for(int m = 0 ; myMoves[m][2] != -1 ; m++){
			makeMove(bs, myMoves[m][0], myMoves[m][1], myMoves[m][2]);
			myMoves[m][3] = -1 * treeSearch(bs, 2);
			undoMove(bs);
			if(myMoves[m][3] > best){
				best = myMoves[m][3];
				b = m;
			}
		}
		makeMove(bs, myMoves[b][0], myMoves[b][1], myMoves[b][2]);
		genFEN(outputFEN, bs);
		printf("%s\n", outputFEN);
	}

	// PERFT_MODE - checks number of positions
	else if (mode == PERFT_MODE){
		printf("Checking board initialization...\n");
		ASSERT(bs -> castlePermission == 0 && bs -> enPas == OFFBOARD);
		printf("Checking movegen test...\n\n");
		ASSERT(testMoves());

		char testFEN[] = "rnbqkbnr/pppppppp/8/8/6P1/8/PPPPPP1P/RNBQKBNR w KQkq g3";
		parseFEN(START_FEN, bs);
		printBoard(bs, OPT_64_BOARD);
		printBoard(bs, OPT_BOARD_STATE);

		int tot = (int)perft(bs, 5);
		// int tot = (int)perft2(bs, 5);
		printf(RED "total: " reset "%i\n", tot);
		printf(RED "\n====== AFTER UNDOS ========\n" reset);
		printBoard(bs, OPT_64_BOARD);
		printBoard(bs, OPT_BOARD_STATE);
	}

	// SEARCH_MODE - output first layer of search
	else if(mode == SEARCH_MODE){
		printf("Checking board initialization...\n");
		ASSERT(bs -> castlePermission == 0 && bs -> enPas == OFFBOARD);
		printf("Checking movegen test...\n\n");
		ASSERT(testMoves());
		char testFEN[] = "4qr1k/6p1/4p2p/p2p2b1/1p2P1Q1/1PrB3P/P2R1PP1/3R2K1 w - -"; // kasparov|karpov
		parseFEN(testFEN, bs);

		int myMoves[218][4];
		int total = genLegalMoves(bs);
		memcpy(myMoves, legalMoves, 218 * sizeof(myMoves[0]));

		for(int m = 0 ; myMoves[m][2] != -1 ; m++){
			printf("evaluating: ");
			printMove(m, myMoves[m][0], myMoves[m][1], myMoves[m][2]);
			makeMove(bs, myMoves[m][0], myMoves[m][1], myMoves[m][2]);
			myMoves[m][3] = -1 * treeSearch(bs, 2);
			undoMove(bs);
		}
		// int m = 0;
		// printf("evaluating: ");
		// printMove(m, myMoves[m][0], myMoves[m][1], myMoves[m][2]);
		// makeMove(bs, myMoves[m][0], myMoves[m][1], myMoves[m][2]);
		// myMoves[m][3] = -1 * treeSearch(bs, 2);
		// undoMove(bs);

		printf(RED "\n====== AFTER UNDOS ========\n" reset);
		printBoard(bs, OPT_64_BOARD);
		printBoard(bs, OPT_BOARD_STATE);
		for(int m = 0 ; myMoves[m][2] != -1 ; m++){
			printf(CYN "move %d eval: " reset "%d\n", m, myMoves[m][3]);
		}
		printf(RED "ischeck count: %d\n" reset, counter);
	}
}
