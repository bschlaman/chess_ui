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
#define FEN3 "1k6/2p5/5p2/3pP3/6Pp/1N6/2K5/8 b - g3 3 3"
#define FEN4 "3k2Q1/7R/1p1p4/p1p2P2/2P1K3/1P3P2/P7/8 b - c6 12 51"

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
enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };
enum { WHITE, BLACK, BOTH };
// use EMPTY for no piece, and also when there is no enPas square
enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK, CANDIDATESQ };

typedef struct {
    int move;
    int castlePerm;
    int enPas;
    int fiftyMove;
    U64 posKey;
} STATE_UNDO;

typedef struct {
		// rename to pieces?
    int board[120];
    int fiftyMove;

		// fen?

    int side;
    int enPas;
    int castlePermission;

    // Hash key, unique representation of board
    U64 posKey;

    // Number of pieces of type [x]

    STATE_UNDO history[50];

    // piece list
    int piecesList[13][10];
    // pList[wN][0] = E1;
    // pList[wN][1] = D4;

} BOARD_STATE;



/* MACROS */
/* GLOBALS */
extern const char pieceChar[];
extern const char castleChar[];
extern const int isPawn[];
extern const int isKing[];
extern const int OFFBOARD;
/* FUNCTIONS */
// fen.c
extern int parseFEN(char *fen, BOARD_STATE *bs);
extern int genFEN(char *fen, BOARD_STATE *bs);
// main.c
extern int sq64to120(int sq64);
extern int sq120to64(int sq120);
extern int frToSq64(int file, int rank);
extern void sqName(char *sqfr, int sq120);
