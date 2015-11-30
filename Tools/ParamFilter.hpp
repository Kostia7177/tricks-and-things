#pragma once

#include "Int2Type.hpp"
#include<functional>

namespace TricksAndThings
{

template<class C, class F, typename P, typename... Args> class ParamFilter;

template<class F, typename Param, typename... Args>
class ParamFilter<Int2Type<true>, F, Param, Args...>
{
    std::function<void(Param &&)> itself;

    public:

    ParamFilter(F &&f, Args &&... args)
        : itself(std::bind(std::forward<F>(f),
                           std::placeholders::_1,
                           std::forward<Args>(args)...)){}

    void operator()(Param &&param)
    { itself(std::forward<Param>(param)); }
};

template<class F, typename Param, typename... Args>
class ParamFilter<Int2Type<false>, F, Param, Args...>
{
    std::function<void()> itself;

    public:

    ParamFilter(F &&f, Args &&... args)
        : itself(std::bind(std::forward<F>(f),
                           std::forward<Args>(args)...)){}

    void operator()(Param &&)   { itself(); }
};

}
