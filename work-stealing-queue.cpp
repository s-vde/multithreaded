
//-----------------------------------------------------------------------------------------------100
/// @file work-stealing-queue.cpp
/// @author Susanne van den Elsen
/// @date 2017
//--------------------------------------------------------------------------------------------------

#include <pthread.h>
#include <array>
#include <atomic>
#include <iostream>

//--------------------------------------------------------------------------------------------------

template <std::size_t size>
class work_stealing_queue
{
public:
    
    work_stealing_queue() : m_jobs({}), m_head(0), m_tail(0), m_mask(size - 1)
    {
        pthread_mutex_init(&m_mutex, NULL);
    }
    
    void push(const int job)
    {
        unsigned tail = m_tail.load();
        if (m_tail < m_head.load() + m_mask) // && tail < max_size    // CORRECT
//        if (tail < m_head.load() + size)  // && tail < max_size         // DATARACE
        {
            m_jobs[tail & m_mask] = std::move(job);
            std::cout << "m_jobs[" << (tail & m_mask) << "]=" << m_jobs[tail & m_mask] << "\n";
            m_tail.exchange(++tail);
            return;
        }
        throw std::runtime_error("Queue is full");
    }
    
    int steal()
    {
        pthread_mutex_lock(&m_mutex);
        unsigned head = m_head.fetch_add(1);
        if (head < m_tail.load())
        {
            int job = m_jobs[head & m_mask];
            pthread_mutex_unlock(&m_mutex);
            return job;
        }
        m_head.fetch_sub(1);
        pthread_mutex_unlock(&m_mutex);
        throw std::runtime_error("No jobs");
    }
    
    void print() const
    {
        for (const auto& job : m_jobs)
        {
            std::cout << job << " ";
        }
        std::cout << "\n";
    }
    
private:
    
    std::array<int,size> m_jobs;
    std::atomic<unsigned> m_head;
    std::atomic<unsigned> m_tail;
    
    std::size_t m_mask;
    
    pthread_mutex_t m_mutex;
    
}; // end struct work_stealing_queue

//--------------------------------------------------------------------------------------------------

work_stealing_queue<4> Q;

//--------------------------------------------------------------------------------------------------

void* master_thread(void* args)
{
    for (int i = 0; i < 10; ++i)
    {
        try
        {
            Q.push(i);
        }
        catch (const std::runtime_error& e) {}
    }
    pthread_exit(0);
}

//--------------------------------------------------------------------------------------------------

void* worker_thread(void* args)
{
    for (int i = 0; i < 10; ++i)
    {
        try
        {
            int job = Q.steal();
        }
        catch (const std::runtime_error& e) {}
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
