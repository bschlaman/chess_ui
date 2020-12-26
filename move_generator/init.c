#include <stdlib.h>
#include <time.h>

void initRand(){
	time_t t;
	srand((unsigned) time(&t));
}
