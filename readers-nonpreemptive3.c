
#include <pthread.h>

/*---------------------------------------------------------------------------75*/
/**
 @file readers-nonpreemptive.c
 @brief Example program from @cite abdulla-popl-14 demonstrating the
 advantage of DPOR<Source> over DPOR<Persistent>.
 @author Susanne van den Elsen
 @date 2015
 */
/*---------------------------------------------------------------------------++*/

// PARAMETERS

#define NUM_THR 3

// GLOBAL VARIABLES

int x[NUM_THR];

// Start Routines

void *writer(void *arg)
{
	x[0] = 1;
	return NULL;
}

void *reader(void *arg)
{
	int id = *(int*) arg;
	int local = x[id];
	local = x[0];
	return NULL;
}

// Main

int main()
{
	pthread_t thr[NUM_THR];
    int tids[NUM_THR];
	for (int i = 0; i < NUM_THR; ++i) {
		tids[i] = i;
		x[i] = 0;
		if (i == 0)	{ pthread_create(thr+0, NULL, writer, NULL);    }
        if (i > 0)  { pthread_create(thr+i, NULL, reader, tids+i);  }
	}
	for (int i = 0; i < NUM_THR; ++i) {
		pthread_join(thr[i], NULL);
	}
	return(0);
}