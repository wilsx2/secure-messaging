// Credit: https://stackoverflow.com/questions/15752659/thread-pooling-in-c11
#include "shared/ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool()
{
    _deconstructing = false;
    const int thread_count = std::thread::hardware_concurrency();
    for(int i = 0; i < thread_count; ++i){
        _threads.emplace_back(std::thread(&ThreadPool::ThreadLoop,this));
    }
}

void ThreadPool::ThreadLoop()
{
    while(true)
    {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock (_queue_mutex);
            _mutex_condition.wait(lock, [this] {
                return !_job_queue.empty() || _deconstructing;
            });
            if(_deconstructing){
                return;
            }
            job = _job_queue.front();
            _job_queue.pop();
        }
        job();
    }
}

void ThreadPool::Enqueue(std::function<void()> job)
{
    {
        std::unique_lock<std::mutex> lock (_queue_mutex);
        _job_queue.push(job);
    }
    _mutex_condition.notify_one();
}

bool ThreadPool::Busy()
{
    bool output;
    {
        std::unique_lock<std::mutex> lock (_queue_mutex);
        output = !_job_queue.empty();
    }
    return output;
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock (_queue_mutex);
        _deconstructing = true;
    }
    _mutex_condition.notify_all();
    for(std::thread& thread : _threads){
        thread.join();
    }
    _threads.clear();
}