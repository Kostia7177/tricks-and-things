#pragma once

#include "detail/SubqueueBase.hpp"
#include "WithParallelConsumers.hpp"
#include "GeneralPurpose.hpp"
#include "detail/ThreadSafeClientHub.hpp"
#include "Traits.hpp"

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

    bool ready()        { return isReady; }
    void push(Type &&p) { itself.push(std::forward<Type>(p)); }
    bool pop(Type &p)   { return itself.pop(p); }
    bool empty() const  { return itself.empty(); }
    size_t size() const { return itself.size(); }

    typedef ThreadSafeClientHub ClientHub;
};

namespace Q2 = Queues;

template<class... Params>
using FewToLot2Traits =
    Q2::Traits
        <
            Q2::UsePolicy<Q2::InfoCallsAre, Template2Type<Q2::WithInfoCalls>>,
            Q2::UsePolicy<Q2::PushWayBalancerIs, Int2Type<true>>,
            Q2::UsePolicy<Q2::PopWayBalancerIs, Int2Type<true>>,
            Params...
        >;

}
namespace Queues
{

template<typename T, class... Params>
using FewToLot2 =
    WithParallelConsumers<detail::FewToLot2Subqueue<T, detail::FewToLot2Traits<Params...>>>;

}
} }
