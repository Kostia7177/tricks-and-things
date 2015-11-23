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

    struct SubCfg : Cfg
    { typedef typename Cfg::SubInfoCalls InfoCalls; };
    Queues::GeneralPurpose<T, SubCfg> itself;
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

}

namespace Queues
{

template<class... Params>
using FewToLot2Traits =
    QueueTraits
        <
            UseQueuePolicy<SubInfoCallsAre, Template2Type<WithInfoCalls>>,
            UseQueuePolicy<PopWayBalancerIs, Int2Type<true>>,
            UseQueuePolicy<PushWayBalancerIs, Int2Type<true>>,
            Params...
        >;

template<typename T, class... Params>
using FewToLot2 =
    WithParallelConsumers<detail::FewToLot2Subqueue<T, FewToLot2Traits<Params...>>>;

template<typename T, class Cfg>
using FewToLot2Preconfigured =
    WithParallelConsumers<detail::FewToLot2Subqueue<T, Cfg>>;

}

} }
