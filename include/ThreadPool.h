#pragma once
#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <thread>
#include <mutex>
#include <vector>
#include <list>
#include "PointEngineExport.hpp"

struct POINTENGINE_API ThreadPool
{
    ThreadPool(size_t threadCount)
    {
        for(int _ = 0; _ < threadCount; _++)
        {
            workers.emplace_back([this](){
                while(true)
                {
                    std::function<void()> job;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        cv.wait(lock, [this](){
                            return stop || !tasks.empty();
                        });
                        if(tasks.empty() && stop) return;
                        job = tasks.front();
                        tasks.pop_front();
                    }
                    job();
                }
            });
        }
    }

    ~ThreadPool()
    {
        stop = true;
        cv.notify_all();
        for(auto& t : workers) t.join();
    }

    void addTask(std::function<void()> task)
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.push_back(task);
        }
        cv.notify_one();
    }

    std::mutex queueMutex;
    std::atomic<bool> stop{false};
    std::vector<std::thread> workers;
    std::condition_variable cv;
    std::deque<std::function<void()>> tasks;

};