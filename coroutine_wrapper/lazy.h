#pragma once

#include <cassert>
#include <coroutine>
#include <utility>

namespace play
{

    template <typename T>
    struct Lazy
    {
        struct PromiseType;
        using Handle = std::coroutine_handle<PromiseType>;
        using promise_type = PromiseType;

        struct PromiseType
        {
            auto initial_suspend()
            {
                return std::suspend_always{};
            }

            auto final_suspend() noexcept
            {
                return std::suspend_always{};
            }

            auto unhandled_exception()
            {
                abort();
            }

            auto get_return_object()
            {
                return Lazy{Handle::from_promise(*this)};
            }

            void return_value(T val)
            {
                data_ = std::move(val);
            }

            T data_;
        };

        T value()
        {
            assert(coro_);
            coro_.resume();
            return std::move(coro_.promise().data_);
        }

        auto operator()()
        {
            return value();
        }

        Lazy(const Lazy &) = delete;
        Lazy(Lazy &&other)
            : coro_(std::move(other.coro_))
        {
            other.coro_ = nullptr;
        }

        ~Lazy()
        {
            if (coro_)
            {
                coro_.destroy();
            }
        }

    private:
        Lazy(Handle h)
            : coro_(h) {}

        Handle coro_;
    };
} // namespace play