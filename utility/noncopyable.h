#pragma once

namespace play
{
    class Nocopyable
    {
    public:
        constexpr Nocopyable() = default;
        ~Nocopyable() = default;
        Nocopyable(const Nocopyable &other) = delete;

        Nocopyable &operator=(const Nocopyable &other) = delete;
    };
} // namespace play