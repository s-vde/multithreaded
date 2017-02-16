
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

   work_stealing_queue() : m_jobs({}), m_head(0), m_tail(0), m_mask(SIZE-1)
   {
      pthread_mutex_init(&m_mutex, NULL);
   }

   void push(const int job)
   {
      unsigned int tail = m_tail.load();
      unsigned int head = m_head.load();

      // if (m_tail < m_mask) // && tail < max_size    // CORRECT
      if (tail < head+SIZE)  // && tail < max_size     // DATARACE
      {
         m_jobs[tail & m_mask] = job;
         m_tail.store(tail+1);
      }
   }

   int steal()
   {
      unsigned int head = m_head.load();
      m_head.store(head+1);

      if (head < m_tail.load())
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

}; // end class work_stealing_queue

//--------------------------------------------------------------------------------------------------

void* master_thread(void* args)
{
   auto* queue = static_cast<work_stealing_queue*>(args);
   for (int i = 0; i < 2; ++i)
   {
      queue->push(i);
   }
   pthread_exit(0);
}

//--------------------------------------------------------------------------------------------------

void* worker_thread(void* args)
{
   auto* queue = static_cast<work_stealing_queue*>(args);
   for (int i = 0; i < 1; ++i)
   {
      int job = queue->steal();
   }
   pthread_exit(0);
}

//--------------------------------------------------------------------------------------------------

int main()
{
   work_stealing_queue queue;
   
   pthread_t master;
   pthread_t worker;

   pthread_create(&master, 0, master_thread, &queue);
   pthread_create(&worker, 0, worker_thread, &queue);

   pthread_join(master, 0);
   pthread_join(worker, 0);

   return 0;
}

//--------------------------------------------------------------------------------------------------
