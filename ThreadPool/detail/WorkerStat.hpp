#pragma once

#include "../../Tools/OneTwo.hpp"

namespace TricksAndThings { namespace detail
{

template<class T, class W> One WithWorkerStatInside(const typename T::template WorkerStat<W> *);
template<typename, typename> Two WithWorkerStatInside(...);

template<class, class, size_t> struct WorkerStat;

template<class C, class W>
struct WorkerStat<C, W, 1>
{
    typedef typename C::template WorkerStat<W> Type;
};

template<class C, class W>
struct WorkerStat<C, W, 0>
{
    struct Type
    {
        template<typename... Args> Type(Args &&...) {}
    };
};

} }
