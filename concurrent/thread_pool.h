#pragma once

#include "concepts.h"
#include "thread_safe_queue.h"
#include "utility/callable_wrapper.h"

#include <atomic>
#include <future>
#include <iostream>
#include <thread>
#include <type_traits>
#include <vector>

namespace play
{
    template <BlockableQueue QueueType = ThreadSafe::Queue<CallableWrapper>>
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

        template <Callable FunctionType>
        auto post(FunctionType func)
            -> std::future<typename std::invoke_result<FunctionType>::type>
        {
            using ResultType = typename std::invoke_result<FunctionType>::type;
            std::packaged_task<ResultType()> task(std::move(func));
            std::future<ResultType> result(task.get_future());
            tasks_.push(std::move(task));
            return result;
        }

        void stop()
        {
#ifdef DEBUG
            std::cout << "ThreadPool::stop()" << std::endl;
#endif
            stop_ = true;
        }

    protected:
        void worker()
        {
            while (!stop_)
            {
                auto task = tasks_.tryPop();
                if (task)
                {
                    (*task)();
                }
                else
                {
                    std::this_thread::yield();
                }
            }
        }

    private:
        std::atomic_bool stop_{false};
        QueueType tasks_{};
        std::vector<std::thread> threads_;
    };

} // namespace play