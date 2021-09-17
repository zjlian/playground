#pragma once

#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace play::ThreadSafe
{
    template <typename Type>
    class Queue
    {
    public:
        using ValueType = Type;

        Queue() = default;
        ~Queue() = default;

        void push(Type value)
        {
            std::lock_guard<std::mutex> lock(mtx_);
            data_.push_back(std::move(value));
            cond_.notify_one();
        }

        Type waitAndPop()
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cond_.wait(lock, [&] {
                return !data_.empty();
            });
            auto value = std::move(data_.front());
            data_.pop_front();
            return value;
        }

        std::optional<Type> tryPop()
        {
            std::lock_guard<std::mutex> lock(mtx_);
            if (data_.empty())
            {
                return {};
            }
            auto value = std::move(data_.front());
            data_.pop_front();
            return value;
        }

        bool empty()
        {
            std::lock_guard<std::mutex> lock(mtx_);
            return data_.empty();
        }

    private:
        mutable std::mutex mtx_;
        std::deque<Type> data_;
        std::condition_variable cond_;
    };
} // namespace play::ThreadSafe