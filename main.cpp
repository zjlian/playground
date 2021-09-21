#include "concepts/base.h"
#include "concurrent/concepts.h"
#include "concurrent/thread_pool.h"
#include "concurrent/thread_safe_queue.h"
#include "utility/callable_wrapper.h"

#include <cassert>
#include <chrono>
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

// 泛型函数，参数 que 必须符合 concept BlockableQueue，que 内的元素必须符合 concept Callable
template <BlockableQueue QueueType>
requires Callable<typename QueueType::ValueType>
void test(QueueType &que)
{
    que.push([] {
        std::cerr << "test" << std::endl;
    });
}

void test(Callable<int32_t, int32_t> auto fn)
{
    int32_t a = 10;
    int32_t b = 20;
    fn(a, b);
    assert(a == 20 && b == 10);
    std::cerr << a << " " << b << std::endl;
}

std::atomic_bool stop{false};

int endloop(const char *msg)
{
    while (!stop)
    {
        std::cerr << msg << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}

void test(CallableWrapper callable)
{
    std::cerr << "call CallableWrapper" << std::endl;
    callable();
}

std::string ThreadIdString()
{
    std::stringstream ss;
    ss << gettid();
    return ss.str();
}

int main()
{
    ThreadPool pool;

    std::vector<std::future<const char *>> results;

    results.push_back(
        pool.post([&] {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            for (size_t i = 0; i < 20; i++)
            {
                pool.post([=] {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    std::cerr << "哈" << i << std::endl;
                });
            }
            return "-";
        }));

    std::this_thread::sleep_for(std::chrono::seconds(10));
}
