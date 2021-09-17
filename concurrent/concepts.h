#pragma once

#include "concepts/base.h"
#include <functional>
#include <memory>

namespace play
{
    template <typename Type>
    concept BlockableQueue = requires(Type t)
    {
        // 需要拥有静态成员 ValueType
        typename Type::ValueType;
        // 需要拥有成员函数 push，接收的参类型是 ValueType 的实例
        t.push(std::move(typename Type::ValueType{}));
        { t.waitAndPop() } -> Same<typename Type::ValueType>;
        { t.tryPop() } -> Same<std::optional<typename Type::ValueType>>;
        { t.empty() } -> Same<bool>;
    };

} // namespace play