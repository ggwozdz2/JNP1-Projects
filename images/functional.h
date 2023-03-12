#ifndef FUNCTIONAL_JNP_H
#define FUNCTIONAL_JNP_H

#include <concepts>
#include <cstdint>
#include <optional>
#include <functional>

template <typename F = nullptr_t>
auto compose()
{
    return [](auto args)
    {
        return args;
    };
}

template <typename F>
auto compose(F func)
{
    return func;
}

template <typename F, typename... Func>
auto compose(F first, Func... others)
{
    return [first, others...](auto args2)
    {
        auto ret_val = first(args2);
        auto recursion = compose(others...);
        return recursion(ret_val);
    };
}

template <typename H>
auto lift(H h)
{
    return [=]([[maybe_unused]] auto args)
    { return h(); };
}

template <typename H, typename... Fs>
auto lift(H h, Fs... fs)
{
    return [h, fs...](auto p)
    {
        return h(fs(p)...);
    };
}

#endif
