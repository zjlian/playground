#pragma once

#include <functional>
#include <type_traits>

namespace play
{

    template <typename T, typename... Args>
    concept Callable = requires(T t, Args... args)
    {
        // t 需要可用被调用和传传参
        t(args...);
    };

    template <typename T, typename U>
    concept Same = std::is_same_v<T, U> && std::is_same_v<U, T>;

} // namespace play