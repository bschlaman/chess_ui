#include <stdlib.h>
#include "colors.h"
#include "defs.h"

int testMoves(){
	BOARD_STATE *tbs = initGame();
	parseFEN(MAXM_FEN, tbs);
	int total = genLegalMoves(tbs);
	free(tbs);
	return total == 218;
}

// TODO: make test that makes and undoes many random moves
