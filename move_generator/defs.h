#include <stdlib.h>

#define DEBUG
#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
if(!(n)){ \
    printf(RED "====== ERROR\n" YEL "%s\n" reset, #n); \
    printf(RED "file: " reset "%s ", __FILE__); \
    printf(RED "line: " reset "%d\n", __LINE__); \
    exit(1); \
}
#endif

enum { false, true };

#define START_FEN  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
// good for white king in check
#define FEN2 "rnbq2n1/ppp2ppp/5Nk1/1b6/4PPp1/2P5/PP2K1PP/RNBQ1BR1 w KQkq f3 2 15"
#define FEN3 "rnkR4/4bq2/p1n4r/Pp1PppNP/1P6/B1PP4/R2K1P2/1N1B4 b KQkq -"
#define FEN4 "3k2Q1/7R/1p1p4/p1p2P2/2P1K3/1P3P2/P7/8 b - c6 12 51"
// castling
#define FEN5 "r3k2r/1p6/8/8/b4Pp1/8/8/R3K2R b KQkq f3"

typedef unsigned long long U64;

enum {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ
};

// 0 0 0 0
enum { WKCA = 8, WQCA = 4, BKCA = 2, BQCA = 1 };
enum { WHITE, BLACK, NEITHER };
enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK, CANDIDATESQ };
// move encoding, using the chess programming wiki method
// 0  0	0	0	0	quiet moves
// 1  0	0	0	1	double pawn push
// 2  0	0	1	0	king castle
// 3  0	0	1	1	queen castle
// 4  0	1	0	0	captures
// 5  0	1	0	1	ep-capture
// 8  1	0	0	0	knight-promotion
// 9  1	0	0	1	bishop-promotion
// 10	1	0	1	0	rook-promotion
// 11	1	0	1	1	queen-promotion
// 12	1	1	0	0	knight-promo capture
// 13	1	1	0	1	bishop-promo capture
// 14	1	1	1	0	rook-promo capture
// 15	1	1	1	1	queen-promo capture
enum { PROMOTION = 8, CAPTURE = 4, SPECIAL1 = 2, SPECIAL2 = 1 };

typedef unsigned short int move;

typedef struct {
	move fromto;
	int enPas;
	int castlePermission;
	int capturedPiece;
} MOVE_STACK;

typedef struct {
	// rename to pieces?
	int board[120];
	int ply;
	
	int side;
	int enPas;
	int castlePermission;
	
	// Hash key, unique representation of board
	U64 posKey;
	
	MOVE_STACK history[200];
} BOARD_STATE;


// global mode
enum { NORMAL_MODE, FEN_MODE, RAND_MODE, SEARCH_MODE };
// printBoard opts
enum { OPT_64_BOARD, OPT_BOARD_STATE, OPT_120_BOARD };

/* MACROS */
/* GLOBALS */
extern int mode;
extern const char pieceChar[];
extern const char castleChar[];
extern const int isPawn[];
extern const int isKing[];
extern const int OFFBOARD;
extern const int numDirections[];
extern const int translation[][8];
// TODO: remove this, temporary workaround
extern int legalMoves[][4];
/* FUNCTIONS */
// fen.c
extern int parseFEN(char *fen, BOARD_STATE *bs);
extern int genFEN(char *fen, BOARD_STATE *bs);
// main.c
extern int sq64to120(int sq64);
extern int sq120to64(int sq120);
extern int frToSq64(int file, int rank);
extern int getType(int piece);
extern int getColor(int piece);
extern void getAlgebraic(char *sqfr, int sq120);
extern void resetBoard(BOARD_STATE *bs);
extern int genLegalMoves(BOARD_STATE *bs);
extern int newBoardCheck(int *board, int sq, int cs);
extern void printMove(int m, int from, int to, int moveType);
extern void printLegalMoves(BOARD_STATE *bs);
// init.c
extern void initRand();
extern BOARD_STATE* initGame();
extern void initLegalMoves();
// moves.c
extern void makeMove(BOARD_STATE *bs, int from, int to, int moveType);
extern void undoMove(BOARD_STATE *bs);
// eval.c
extern int randInt(int lb, int ub);
extern int negaMax(BOARD_STATE *bs, int depth);
extern int eval(BOARD_STATE *bs);
extern int treeSearch(BOARD_STATE *bs, int depth);
