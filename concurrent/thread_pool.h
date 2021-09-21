#pragma once

#include "concepts.h"
#include "thread_safe_deque.h"
#include "thread_safe_queue.h"
#include "utility/callable_wrapper.h"

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <future>
#include <iostream>
#include <memory>
#include <optional>
#include <queue>
#include <thread>
#include <vector>

namespace play
{
    template <
        BlockableQueue QueueType = ThreadSafe::Queue<CallableWrapper>,
        BlockableDeque LocalQueueType = ThreadSafe::Deque<CallableWrapper>>
    requires Callable<typename QueueType::ValueType> &&
        Callable<typename LocalQueueType::ValueType>
    class ThreadPool
    {
        using TaskType = typename QueueType::ValueType;

    public:
        ThreadPool()
        {
            auto thread_count = std::thread::hardware_concurrency();
            local_tasks_list_.reserve(thread_count);
            threads_.reserve(thread_count);
            for (size_t i = 0; i < thread_count; i++)
            {
                local_tasks_list_.emplace_back(std::make_shared<LocalQueueType>());
                threads_.emplace_back([&, i] {
                    worker(i);
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

        /// @brief 提交新的任务到线程池
        /// @param func 可调用对象，
        template <Callable FunctionType>
        auto post(FunctionType func)
            -> std::future<typename std::invoke_result<FunctionType>::type>
        {
            using ResultType = typename std::invoke_result<FunctionType>::type;
            std::packaged_task<ResultType()> task(std::move(func));
            std::future<ResultType> result(task.get_future());
            if (local_tasks_)
            {
                local_tasks_->push(std::move(task));
            }
            else
            {
                tasks_.push(std::move(task));
            }
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
        /// 线程工作函数
        void worker(uint32_t index)
        {
            index_ = index;
            local_tasks_ = local_tasks_list_[index_];

            while (!stop_)
            {
                nextTask();
            }
        }

        /// 执行下一个任务
        void nextTask()
        {
            auto task = getLocalTask();
            if (task)
            {
                (*task)();
                return;
            }

            task = getMainTask();
            if (task)
            {
                (*task)();
                return;
            }

            task = getStolenTask();
            if (task)
            {
                (*task)();
                return;
            }
            std::this_thread::yield();
        }

        std::optional<TaskType> getMainTask()
        {
            return tasks_.tryPop();
        }

        std::optional<TaskType> getLocalTask()
        {
            if (local_tasks_)
            {
                return local_tasks_->tryPop();
            }
            return {};
        }

        std::optional<TaskType> getStolenTask()
        {
            std::optional<TaskType> task{};
            for (size_t i = 0; i < local_tasks_list_.size(); i++)
            {
                const size_t target_index = (index_ + i + 1) % local_tasks_list_.size();
                task = local_tasks_list_[target_index]->tryPopBack();
                if (task)
                {
                    return task;
                }
            }
            return {};
        }

    private:
        std::atomic_bool stop_{false};
        QueueType tasks_{};
        std::vector<std::shared_ptr<LocalQueueType>> local_tasks_list_;
        inline static thread_local std::shared_ptr<LocalQueueType> local_tasks_;
        inline static thread_local uint32_t index_;
        std::vector<std::thread> threads_;
    };

} // namespace play
