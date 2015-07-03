/*
    Use free. Be free. Be lock-free!!

    Copyright (C) 2014, 2015  Konstantin U. Zozoulia

    candid.71 -at- mail -dot- ru

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include<algorithm>
#include<stdexcept>
#include<sstream>

namespace TricksAndThings { namespace LockFree { namespace Queues
{

template<class Subqueue, class Balancer>
Subqueue *WithParallelConsumers<Subqueue, Balancer>::selectSubqueue(size_t *idxRet)
{
    do
    {
        if (!numOfConsumers) { return 0; }

        if (!balancer.pop(idxRet))
        {
            typename Subqueue::ClientHub::Ptr clientHubPtr = &clientHub;
            size_t idx = clientHubPtr->getConsumerIdx();
            do
            {
                *idxRet = idx ++ ;
                if (!subqueues[idx].ready()) { idx = 0; }
            }
            while (!clientHubPtr->syncIdx(&idx));
        }
    }
    while (exitedConsumersMap.contains(*idxRet));

    return &subqueues[*idxRet];
}

template<class Subqueue, class Balancer>
Subqueue *WithParallelConsumers<Subqueue, Balancer>::getSubqueue()
{
    size_t idx;
    if (exitedConsumersMap.pop(&idx))
    {
        ++ numOfConsumers;
        return &subqueues[idx];
    }
    for (idx = 0; idx < Subqueue::Cfg::numOfConsumersLimit; ++ idx)
    {
        Subqueue *ptr = &subqueues[idx];
        if (ptr->acquire())
        {
            if (!ptr->ready()) { ptr->init(&numOfConsumers); }
            return ptr;
        }
    }

    return 0;
}

template<class Subqueue, class Balancer>
WithParallelConsumers<Subqueue, Balancer>::WithParallelConsumers(size_t n)
    : numOfConsumers(0)
{
    if (numOfConsumers > sizeof(subqueues) / sizeof(Subqueue))
    {
        std::stringstream exc;
        exc << "Number of consumers cannot be greater than";
        throw std::runtime_error(exc.str());
    }

    std::for_each(subqueues, subqueues + n, [&](Subqueue &subqueue)
                                            { subqueue.init(&numOfConsumers); });
}

template<class Subqueue, class Balancer>
WithParallelConsumers<Subqueue, Balancer>::ConsumerSideProxy::ConsumerSideProxy(WithParallelConsumers *q)
    : queue(q),
      threadId(std::this_thread::get_id())
{
    if (!queue)
    {
        throw std::runtime_error("Consumer side proxy pointer "
                                 "to a parallel-consumers queue "
                                 "cannot be initialized by zero!");
    }

    subqueue = queue->getSubqueue();
}

template<class Subqueue, class Balancer>
WithParallelConsumers<Subqueue, Balancer>::ConsumerSideProxy::~ConsumerSideProxy()
{
    -- queue->numOfConsumers;
    queue->exitedConsumersMap.push(queue->getIdx(subqueue));
}

template<class Subqueue, class Balancer>
bool WithParallelConsumers<Subqueue, Balancer>::ConsumerSideProxy::pop(Type &p)
{
    std::thread::id caller = std::this_thread::get_id();
    if (threadId != caller)
    {
        std::stringstream exc;
        exc << "Queue consumer, owned by thread id " << threadId
            << ", was called from thread id " << caller
            << "; ";
        throw std::runtime_error(exc.str());
    }

    bool ret = subqueue->pop(p);
    if (ret) { queue->decrSize(); }

    return ret;
}

template<class Subqueue, class Balancer>
WithParallelConsumers<Subqueue, Balancer>::ProviderSideProxy::ProviderSideProxy(WithParallelConsumers *q)
    : queue(q),
      idx(0)
{
    if (!queue)
    {
        throw std::runtime_error("Provider side proxy pointer "
                                 "to a parallel-comsumers queue "
                                 "cannot be initialized by zero!");
    }

    queue->clientHub.onNewProvider();
}

template<class Subqueue, class Balancer>
WithParallelConsumers<Subqueue, Balancer>::ProviderSideProxy::~ProviderSideProxy()
{
    queue->clientHub.onProviderExited();
}

template<class Subqueue, class Balancer>
void WithParallelConsumers<Subqueue, Balancer>::ProviderSideProxy::push(Type &&p)
{
    if (Subqueue *subqueue = queue->selectSubqueue(&idx))
    {
        subqueue->push(std::forward<Type>(p));
        queue->incrSize();
    }
    else { throw std::runtime_error("No initialized sub-queue found!"); }
}

template<class Subqueue, class Balancer>
template<class F>
void WithParallelConsumers<Subqueue, Balancer>::ConsumerIdle::until(F f)
{
    std::unique_lock<std::mutex> locker(lock);
    awaiting = true;
    check.wait(locker, f);
    awaiting = false;
}

template<class Subqueue, class Balancer>
void WithParallelConsumers<Subqueue, Balancer>::ConsumerIdle::interrupt()
{
    if (awaiting) { kick(); }
}

} } }
