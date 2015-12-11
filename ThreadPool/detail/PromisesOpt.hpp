#pragma once

#include<exception>
#include<future>

namespace TricksAndThings { namespace detail
{

struct NoPromise
{
    template<class F>
    void operator()(const F &f)
    { f(); }
};

template<typename Ret>
class WithPromise
{
    std::promise<Ret> promise;

    public:

    WithPromise() = default;

    WithPromise(WithPromise &&p)
    { swap(promise, p.promise); }

    template<class F>
    void operator()(const F &f)
    {
        try { promise.set_value(f()); }
        catch (...)
        { promise.set_exception(std::current_exception()); }
    }

    std::future<Ret> getFuture()
    { return promise.get_future(); }
};

} }
