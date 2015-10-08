#include "random_number.h"

extern "C" {
	#include <stdlib.h>
	#include <sys/time.h>
	#include <time.h>
	#include <unistd.h>
}

int randomNumber(int max){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(time(NULL)+getpid()+tv.tv_usec);
	return rand() % max;
}
