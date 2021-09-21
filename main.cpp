#include "concepts/base.h"
#include "concurrent/concepts.h"
#include "concurrent/thread_pool.h"
#include "concurrent/thread_safe_queue.h"
#include "utility/callable_wrapper.h"

#include <cassert>
#include <chrono>
#include <coroutine>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <future>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <thread>
#include <type_traits>
#include <vector>

using namespace play;

struct HelloCoroutine
{
    struct HelloPromise
    {
        HelloCoroutine get_return_object()
        {
            return std::coroutine_handle<HelloPromise>::from_promise(*this);
        }
        std::suspend_never initial_suspend()
        {
            return {};
        }
        // 在 final_suspend() 挂起了协程，所以要手动 destroy
        std::suspend_always final_suspend() noexcept
        {
            return {};
        }
        void unhandled_exception() {}
    };

    using promise_type = HelloPromise;
    HelloCoroutine(std::coroutine_handle<HelloPromise> h)
        : handle(h) {}

    std::coroutine_handle<HelloPromise> handle;
};

HelloCoroutine hello()
{
    std::cout << "Hello " << std::endl;
    co_await std::suspend_always{};
    std::cout << "Coroutine " << std::endl;
}

int main()
{
    HelloCoroutine co = hello();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    co.handle.resume();
}
