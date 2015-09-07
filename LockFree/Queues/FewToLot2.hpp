#pragma once

#include "../detail/SubqueueBase.hpp"
#include "WithParallelConsumers.hpp"
#include "GeneralPurpose.hpp"
#include "../detail/ThreadSafeClientHub.hpp"

namespace TricksAndThings { namespace LockFree
{
namespace detail {
template<typename T, class C>
class FewToLot2Subqueue
    : public SubqueueBase<C>
{
    public:
    typedef T Type;
    typedef C Cfg;
    private:
    Queues::GeneralPurpose<T, Cfg> itself;
    std::atomic<bool> isReady;
    public:
    FewToLot2Subqueue() : isReady(false){}
    void init(SizeAtomic *numOfConsumers)
    {
        isReady = true;
        ++ *numOfConsumers;
    }
    bool ready(){ return isReady; }
    void push(Type &&p){ itself.push(std::forward<Type>(p)); }
    bool pop(Type &p){ return itself.pop(p); }

    typedef ThreadSafeClientHub ClientHub;
};
}
namespace Queues
{
template<typename T, class Cfg = Traits<DefaultAllocatingStorage,Components::WithInfoCalls>> using FewToLot2 = WithParallelConsumers<detail::FewToLot2Subqueue<T, Cfg>, pushWayLookup, popWayLookup>;
}
} }
