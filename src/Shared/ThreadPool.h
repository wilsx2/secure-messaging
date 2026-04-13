#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool
{
    private:
    std::queue<std::function<void()>> _job_queue;
    std::mutex _queue_mutex;
    std::condition_variable _mutex_condition;
    std::vector<std::thread> _threads;
    std::size_t _max_threads;
    bool _deconstructing;

    void ThreadLoop();
    
    public:
    ThreadPool();
    ~ThreadPool();
    void Enqueue(std::function<void()> job);
    bool Busy();
};