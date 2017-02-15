
//-----------------------------------------------------------------------------------------------100
/// @file work-stealing-queue.cpp
/// @author Susanne van den Elsen
/// @date 2017
//--------------------------------------------------------------------------------------------------

#include <pthread.h>
#include <array>
#include <atomic>

#define SIZE 1

//--------------------------------------------------------------------------------------------------

class work_stealing_queue
{
public:

   work_stealing_queue() : m_jobs({}), m_head(0), m_tail(0), m_mask(SIZE - 1)
   {
      pthread_mutex_init(&m_mutex, NULL);
   }

   void push(const int job)
   {
      unsigned int tail = m_tail.load();
      unsigned int head = m_head.load();

      // if (m_tail < m_head.load() + m_mask) // && tail < max_size    // CORRECT
      if (tail < head+SIZE)  // && tail < max_size         // DATARACE
      {
         m_jobs[tail & m_mask] = job;
         m_tail.store(tail+1);
      }
   }

   int steal()
   {
      unsigned int head = m_head.load();
      m_head.store(head+1);

      unsigned int tail = m_tail.load();
      if (head < tail)
      {
         int job = m_jobs[head & m_mask];
         return job;
      }
      return -1;
   }

private:

   std::array<int,SIZE> m_jobs;
   std::atomic<unsigned int> m_head;
   std::atomic<unsigned int> m_tail;

   size_t m_mask;

   pthread_mutex_t m_mutex;

}; // end struct work_stealing_queue

//--------------------------------------------------------------------------------------------------

work_stealing_queue Q;

//--------------------------------------------------------------------------------------------------

void* master_thread(void* args)
{
   for (int i = 0; i < 2; ++i)
   {
      Q.push(i);
   }
   pthread_exit(0);
}

//--------------------------------------------------------------------------------------------------

void* worker_thread(void* args)
{
   for (int i = 0; i < 1; ++i)
   {
      int job = Q.steal();
   }
   pthread_exit(0);
}

//--------------------------------------------------------------------------------------------------

int main()
{
   pthread_t master;
   pthread_t worker;

   pthread_create(&master, 0, master_thread, 0);
   pthread_create(&worker, 0, worker_thread, 0);

   pthread_join(master, 0);
   pthread_join(worker, 0);

   return 0;
}

//--------------------------------------------------------------------------------------------------
