#pragma once

#include<thread>
#include "WithParallelConsumers.hpp"
#include "Traits.hpp"
#include "FewToLot2.hpp"
#include "../../ThreadPool/detail/RecurserIfc.hpp"

namespace TricksAndThings { namespace LockFree
{

namespace detail
{

typedef TricksAndThings::detail::RecurserPtr RecurserPtr;

template<typename T, class SubqueueCore>
class WithParallelConsumersRecursive
    : public Queues::WithParallelConsumers<SubqueueCore>
{
    typedef Queues::WithParallelConsumers<SubqueueCore> Core;

    public:

    typedef typename SubqueueCore::Type Type;

    WithParallelConsumersRecursive(size_t s = 0) : Core(s) {}

    class Recurser;

    class ConsumerSideProxy
        : public Core::ConsumerSideProxy
    {
        public:

        ConsumerSideProxy(WithParallelConsumersRecursive *p)
            : Core::ConsumerSideProxy(p) {}

        RecurserPtr getRecurser()
        {
            return RecurserPtr(new Recurser(this));
        }
    };

    class Recurser
        : public TricksAndThings::detail::RecurserIfc
    {
        friend RecurserPtr ConsumerSideProxy::getRecurser();

        ConsumerSideProxy *consumerSideProxy;

        typename Core::Cfg::template PushOverrider<ConsumerSideProxy> condition;

        Recurser(ConsumerSideProxy *p)
            : consumerSideProxy(p),
              condition(consumerSideProxy) {}

        public:

        bool overridePush()
        { return condition(); }
    };
};
}

namespace Queues {
template<typename T, class... Params>
using FewToLotRecursive =
    detail::WithParallelConsumersRecursive<T,detail::FewToLot2Subqueue<T, FewToLot2Traits<Params...>>>;

template<typename T, class Cfg = FewToLot2Traits<>>
using FewToLotRecursivePreconfigured =
    detail::WithParallelConsumersRecursive<T, detail::FewToLot2Subqueue<T, Cfg>>;
}
} }
