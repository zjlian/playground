#include "concepts/base.h"
#include "concurrent/concepts.h"
#include "concurrent/thread_pool.h"
#include "concurrent/thread_safe_queue.h"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <sys/types.h>
#include <thread>
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

void endloop(const char *msg)
{
    while (!stop)
    {
        std::cout << msg << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main()
{
    ThreadPool pool;

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    pool.post([] {
        endloop("1111");
    });

    pool.post([] {
        endloop("2222");
    });

    pool.post([] {
        endloop("3333");
    });

    pool.post([] {
        endloop("4444");
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    pool.stop();
    stop = true;
}
