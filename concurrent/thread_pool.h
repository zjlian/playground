#pragma once

#include "concepts.h"
#include "thread_safe_queue.h"

#include <atomic>
#include <thread>
#include <vector>

namespace play
{
    template <BlockableQueue QueueType = ThreadSafe::Queue<std::function<void()>>>
    requires Callable<typename QueueType::ValueType>
    class ThreadPool
    {
        using TaskType = typename QueueType::ValueType;

    public:
        ThreadPool()
        {
            auto thread_count = std::thread::hardware_concurrency();
            threads_.reserve(thread_count);
            for (size_t i = 0; i < thread_count; i++)
            {
                threads_.emplace_back([&] {
                    worker();
                });
            }
        }

        ~ThreadPool()
        {
            stop();
            for (auto &thr : threads_)
            {
                if (thr.joinable())
                {
                    thr.join();
                }
            }
        }

        void submit(TaskType task)
        {
            tasks_.push(task);
        }

        void stop()
        {
            stop_ = true;
        }

    protected:
        void worker()
        {
            while (!stop_)
            {
                tasks_.waitAndPop()();
            }
        }

    private:
        std::atomic_bool stop_{false};
        QueueType tasks_{};
        std::vector<std::thread> threads_;
    };

} // namespace play