#include "concepts/base.h"
#include "concurrent/concepts.h"
#include "concurrent/thread_pool.h"
#include "concurrent/thread_safe_queue.h"
#include "utility/callable_wrapper.h"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <iostream>
#include <sys/types.h>
#include <thread>
#include <type_traits>
#include <vector>

using namespace play;

// 泛型函数，参数 que 必须符合 concept BlockableQueue，que 内的元素必须符合 concept Callable
template <BlockableQueue QueueType>
requires Callable<typename QueueType::ValueType>
void test(QueueType &que)
{
    que.push([] {
        std::cout << "test" << std::endl;
    });
}

void test(Callable<int32_t, int32_t> auto fn)
{
    int32_t a = 10;
    int32_t b = 20;
    fn(a, b);
    assert(a == 20 && b == 10);
    std::cout << a << " " << b << std::endl;
}

std::atomic_bool stop{false};

int endloop(const char *msg)
{
    while (!stop)
    {
        std::cout << msg << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}

void test(CallableWrapper callable)
{
    std::cout << "call CallableWrapper" << std::endl;
    callable();
}

int main()
{
    ThreadPool pool;

    std::vector<std::future<const char *>> results;

    results.push_back(
        pool.post([] {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            return "哈哈";
        }));

    results.push_back(
        pool.post([] {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            return "哦哦";
        }));

    results.push_back(
        pool.post([] {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            return "嘻嘻";
        }));

    for (auto &rt : results)
    {
        std::cout << rt.get() << std::endl;
    }
}
