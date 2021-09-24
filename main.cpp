#include "concepts/base.h"
#include "concurrent/concepts.h"
#include "concurrent/thread_pool.h"
#include "concurrent/thread_safe_queue.h"
#include "utility/callable_wrapper.h"

#include "coroutine_wrapper/generator.h"
#include "coroutine_wrapper/lazy.h"

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

Generator<bool> func()
{
    std::string str = "Hello World";
    int32_t i = 0;
    for (; i < str.length(); i++)
    {
        std::cout << str[i] << std::ends;
        co_yield true;
    }
    std::cout << std::endl;
}

int64_t fibImpl(int32_t n)
{
    if (n <= 2)
    {
        return 1;
    }
    return fibImpl(n - 2) + fibImpl(n - 1);
}

Lazy<int64_t> fib(int32_t n)
{
    co_return fibImpl(n);
}

int main()
{
    auto lazy = fib(10);
    std::cout << "===" << std::endl;
    std::cout << lazy() << std::endl;
}
