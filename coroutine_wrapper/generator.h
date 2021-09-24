#pragma once

#include <cassert>
#include <coroutine>
#include <utility>

namespace play
{
    /// @brief 基本协程包装类型，仅支持 co_yield
    template <typename T>
    struct Generator
    {
        struct PromiseType;

        /// std::coroutine_handle 协程的控制器接口，PromiseType为具体实现
        using HandleType = std::coroutine_handle<PromiseType>;

        /// promise_type c++协程要求的对协程控制器接口的实现
        using promise_type = PromiseType;

        struct PromiseType
        {
            /// @brief 协程函数调用时的实际返回值
            /// 可以是 std::coroutine_handle，将协程的控制器直暴露给用户
            /// 这里返回的是控制器的包装类型 Generator，对协程的控制器进行了一定的封装
            /// 该函数返回后，协程便创建成功
            auto get_return_object()
            {
                return Generator{HandleType::from_promise(*this)};
            }

            /// @brief 协程初始化后是否挂起
            /// std::suspend_always{} 挂起
            /// std::suspend_never{} 不挂起
            auto initial_suspend()
            {
                return std::suspend_always{};
            }

            /// @brief 协程执行完后是否挂起
            auto final_suspend() noexcept
            {
                return std::suspend_always{};
            }

            /// @brief 异常处理
            void unhandled_exception()
            {
                abort();
            }

            void return_void() {}

            auto yield_value(T val)
            {
                data_ = std::move(val);
                return std::suspend_always{};
            }

            T data_;
        };

        Generator(const Generator &) = delete;

        Generator(Generator &&other)
            : coro_(std::move(other.coro_))
        {
            other.coro_ = nullptr;
        }

        bool hasNext()
        {
            return coro_ && !coro_.done();
        }

        void resume()
        {
            if (hasNext())
            {
                coro_.resume();
            }
        }

        T value()
        {
            return std::move(coro_.promise().data_);
        }

        operator bool()
        {
            return hasNext();
        }

        T operator()()
        {
            assert(hasNext());
            coro_.resume();
            return std::move(value());
        }

    private:
        Generator(HandleType h)
            : coro_(h) {}

        /// 协程的控制器
        HandleType coro_;
    };
} // namespace play