#pragma once

#include "concepts/base.h"
#include "utility/noncopyable.h"

#include <memory>

namespace play
{

    /// @brief 禁用拷贝的可调用对象的类型消除包装
    class CallableWrapper : public Nocopyable
    {
        class ImplementBase
        {
        public:
            virtual void call() = 0;
            virtual ~ImplementBase() = default;
        };

        template <Callable Func>
        class Implement : public ImplementBase
        {
        public:
            explicit Implement(Func &&f)
                : fn(std::move(f))
            {
            }

            void call() override
            {
                fn();
            }

        private:
            Func fn;
        };

    public:
        template <Callable Func>
        CallableWrapper(Func &&f)
            : impl_(std::make_unique<Implement<Func>>(std::move(f))) {}

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
        std::unique_ptr<ImplementBase> impl_;
    };

} // namespace play