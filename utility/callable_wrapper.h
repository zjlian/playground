#pragma once

#include "concepts/base.h"
#include "utility/noncopyable.h"

#include <memory>

namespace play
{

    /// @brief 禁用拷贝的可调用对象的类型消除包装
    class CallableWrapper : public Nocopyable
    {
        struct Base
        {
            virtual void call() = 0;
            virtual ~Base() = 0;
        };

        template <Callable Func>
        struct Implement : Base
        {
            Func fn;

            explicit Implement(Func &&f)
                : fn(std::move(f))
            {
            }

            void call() override
            {
                fn();
            }
        };

    public:
        template <Callable Func>
        explicit CallableWrapper(Func &&f)
            : impl_(std::make_unique<Func>(std::move(f))) {}

        void operator()()
        {
            impl_->call();
        }

        CallableWrapper(CallableWrapper &&other) noexcept
            : impl_(std::move(other.impl_)) {}

        CallableWrapper &operator=(CallableWrapper &&other) noexcept
        {
            impl_ = std::move(other.impl_);
            return *this;
        }

    private:
        std::unique_ptr<Base> impl_;
    };

} // namespace play