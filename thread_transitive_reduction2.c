
#include <pthread.h>

/*---------------------------------------------------------------------------75*/
/**
 @file thread_transitive_reduction.c
 @brief Example program from @cite coons-thesis demonstrating the 
 state-space-reduction obtained from thread-transitive reduction.
 @details The quotient of the state-space w.r.t. the HappensBefore relation
 using the standard Dependence relation consists of
 { [000111, error=0], [011100, error=1] }.
 Without thread-transitive reduction DPOR<Persistent> and DPOR<BoundPersistent>
 explore both 011100 and 111000 from the same equivalence class.
 @author Susanne van den Elsen
 @date 2016
 */
/*---------------------------------------------------------------------------++*/

// PARAMETERS

#define NUM_THR 2
#define NUM_VARS 4

// GLOBAL VARIABLES

int x, x_init, a1, a2, error = 0;

// Start Routines

void *thread0(void* arg)
{
    x = 1;
    x_init = 1;
    a1 = 1;
    pthread_exit(0);
}

void *thread1(void* arg)
{
    a2 = 1;
    if (x_init == 1) {
        x = 2;
    } else {
        error = 1;
    }
    pthread_exit(0);
}

// Main

int main()
{
    pthread_t thr[NUM_THR];
    int tids[NUM_THR];
    for (int i = 0; i < NUM_THR; ++i) {
        tids[i] = i;
    }
    pthread_create(thr + 0, NULL, thread0, tids + 0);
    pthread_create(thr + 1, NULL, thread1, tids + 1);

    pthread_join(thr[0], NULL);
    pthread_join(thr[1], NULL);
    return(0);
}
