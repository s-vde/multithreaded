
//-----------------------------------------------------------------------------------------------100
/// @file lazy-dining-philosophers-single-meal.c
/// @details Variant of the classic Dining Philosophers scenario where a number of philosophers are
/// at a dining table trying to grab a single portion of spaghetti. For that they need two forks:
/// the one to their left, and the one to their right. Each of the forks can be held by one
/// philosopher at a time. Lazy as they are, before taking on either fork, they check whether they
/// still have a chance of taking the dish.
/// @note This program has a potential deadlock.
/// @author Susanne van den Elsen
/// @date 2017
//--------------------------------------------------------------------------------------------------

#include <pthread.h>
#include <array>
#include <atomic>
#include <iostream>

//--------------------------------------------------------------------------------------------------

struct fork
{
    fork()
    {
        pthread_mutex_init(&m_mutex, NULL);
    }
    
    void take_up()
    {
        pthread_mutex_lock(&m_mutex);
    }
    
    void put_down()
    {
        pthread_mutex_unlock(&m_mutex);
    }
    
private:
    
    pthread_mutex_t m_mutex;
    
}; // end struct fork

//--------------------------------------------------------------------------------------------------

std::array<fork,NR_THREADS> forks;
std::atomic<bool> meal_taken;

//--------------------------------------------------------------------------------------------------

/// @brief Philosopher thread start routine

void* lazy_philosopher(void* args)
{
    int id = *(int*) args;
    int left = id;
    int right = (id+1) % NR_THREADS;
    
    if (!meal_taken.load())
    {
        forks[left].take_up();
        
        if (!meal_taken.load())
        {
            forks[right].take_up();
            
            bool expected = false;
            bool success = meal_taken.compare_exchange_weak(expected, true);
            if (success)
            {
                std::cout << "philosopher " << id << " enjoys a meal\n";
            }
            
            forks[right].put_down();
        }
        
        forks[left].put_down();
    }
    
    pthread_exit(0); 
} 

//--------------------------------------------------------------------------------------------------

int main()
{
    pthread_t phils[NR_THREADS];
	int pid[NR_THREADS];
    meal_taken.store(0);
    
    // initialize
    for (int i = 0; i < NR_THREADS; ++i)
    {
    	pid[i] = i;
    	forks[i] = fork();
    }
    
    // spawn philosopher threads
    for (int i = 0; i < NR_THREADS; ++i)
    {
        pthread_create(phils + i, 0, lazy_philosopher, pid + i);
    }
    
    // join philosopher threads
    for (int i = 0; i < NR_THREADS; ++i)
    {
        pthread_join(phils[i], NULL);
    }
    
    return(0); 
}

//--------------------------------------------------------------------------------------------------
