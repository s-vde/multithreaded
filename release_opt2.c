
#include <pthread.h>

/*---------------------------------------------------------------------------75*/
/**
 @file release_opt.c
 @brief Example program from @cite coons-thesis demonstrating that 
 DPOR<BoundPersistenSet<Preemptions>> requires preemption prior to an UNLOCK
 operation.
 @details 00*11*0^11 is equivalent to 0^1*00^111 but the latter requires
 one preemption less. By scheduling Thread 1 prior to Thread 0's UNLOCK
 operation, the search ensures that Thread 1's LOCK operation blocks and it
 can perform a context-switch for free.
 @note In this case, 1*000^111 is another equivalent execution with only a 
 single preemption.
 @author Susanne van den Elsen
 @date 2016
 */
/*---------------------------------------------------------------------------++*/

// PARAMATERS

#define NUM_THR 2
#define NUM_LOCKS 1

// GLOBAL VARIABLES

pthread_mutex_t locks[NUM_LOCKS];
int x = 0;

// Start Routines

void *thread0(void *arg)
{
    pthread_mutex_lock(locks + 0);
    pthread_mutex_unlock(locks + 0);
    x = 1;
    pthread_exit(0);
}

void *thread1(void *arg)
{
    x = 0;                              // reset x
    pthread_mutex_lock(locks + 0);
    x = 2;
    pthread_mutex_unlock(locks + 0);
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
