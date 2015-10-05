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

template<class Subqueue, PushWayBalancer b1, PopWayBalancer b2, class M>
template<typename T>
void WithParallelConsumers<Subqueue, b1, b2, M>::Itself::DoLookup::tryFix(bool &fetched, T &p)
{
    size_t idx;
    // first, look, wether there is any queue left by
    // it's consumer
    if (subj->exitedConsumersMap.getLowest1(&idx))
    {
        fetched = subj->subqueues[idx].pop(p);
    }

    if (!fetched
        && subj->workloadMap.getLowest0(&idx))
    {
        fetched = subj->subqueues[idx].pop(p);
    }

    if (fetched)
    {
        subj->pushWayBalancer.push(idx);
    }
}
template<class Subqueue, PushWayBalancer b1, PopWayBalancer b2, class M>
bool WithParallelConsumers<Subqueue, b1, b2, M>::Itself::DoLookup::pop(size_t *idxRet)
{
    do
    {
        if (!subj->workloadMap.pop(idxRet)) { return false; }
    }
    while (subj->exitedConsumersMap.contains(*idxRet));
}

template<class Subqueue, PushWayBalancer b1, PopWayBalancer b2, class M>
Subqueue *WithParallelConsumers<Subqueue, b1, b2, M>::Itself::selectSubqueue(size_t *idxRet)
{
    if (!numOfConsumers) { return 0; }

    if (!pushWayBalancer.pop(idxRet))
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

    return &subqueues[*idxRet];
}

template<class Subqueue, PushWayBalancer b1, PopWayBalancer b2, class M>
Subqueue *WithParallelConsumers<Subqueue, b1, b2, M>::Itself::getSubqueue()
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

template<class Subqueue, PushWayBalancer b1, PopWayBalancer b2, class M>
WithParallelConsumers<Subqueue, b1, b2, M>::Itself::Itself(size_t n)
    : numOfConsumers(0),
      workloadMap(subqueues),
      pushWayBalancer(this),
      popWayBalancer(this)
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
template<class Subqueue, PushWayBalancer b1, PopWayBalancer b2, class M>
WithParallelConsumers<Subqueue, b1, b2, M> &WithParallelConsumers<Subqueue, b1, b2, M>::operator=(WithParallelConsumers &&p)
{
    itself = std::move(p.itself);
    return *this;
}

template<class Subqueue, PushWayBalancer b1, PopWayBalancer b2, class M>
WithParallelConsumers<Subqueue, b1, b2, M>::ConsumerSideProxy::ConsumerSideProxy(WithParallelConsumers *q)
    : queue(q->itself),
      threadId(std::this_thread::get_id())
{
    if (!queue)
    {
        throw std::runtime_error("Consumer side proxy pointer "
                                 "to a parallel-consumers queue "
                                 "cannot be initialized by zero!");
    }

    subqueue = queue->getSubqueue();
    subqueueIdx = subqueue - queue->subqueues;
}

template<class Subqueue, PushWayBalancer b1, PopWayBalancer b2, class M>
WithParallelConsumers<Subqueue, b1, b2, M>::ConsumerSideProxy::~ConsumerSideProxy()
{
    -- queue->numOfConsumers;
    queue->workloadMap.erase(subqueueIdx);
    queue->exitedConsumersMap.push(subqueueIdx);
}

template<class Subqueue, PushWayBalancer b1, PopWayBalancer b2, class M>
bool WithParallelConsumers<Subqueue, b1, b2, M>::ConsumerSideProxy::pop(Type &p)
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

    bool fetched = subqueue->pop(p);
    if (fetched)
    {
        queue->pushWayBalancer.push(subqueueIdx);
    }
    else
    {
        queue->popWayBalancer.tryFix(fetched, p);
    }
    if (fetched) { queue->decrSize(); }

    return fetched;
}

template<class Subqueue, PushWayBalancer b1, PopWayBalancer b2, class M>
WithParallelConsumers<Subqueue, b1, b2, M>::ProviderSideProxy::ProviderSideProxy(WithParallelConsumers *q)
    : queue(q->itself),
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

template<class Subqueue, PushWayBalancer b1, PopWayBalancer b2, class M>
void WithParallelConsumers<Subqueue, b1, b2, M>::ProviderSideProxy::push(Type &&p)
{
    if (Subqueue *subqueue = queue->selectSubqueue(&idx))
    {
        subqueue->push(std::forward<Type>(p));
        queue->incrSize();
    }
    else { throw std::runtime_error("No initialized sub-queue found!"); }
}

template<class Subqueue, PushWayBalancer b1, PopWayBalancer b2, class M>
template<class F>
void WithParallelConsumers<Subqueue, b1, b2, M>::ConsumerIdle::until(F f)
{
    std::unique_lock<std::mutex> locker(lock);
    awaiting = true;
    check.wait(locker, f);
    awaiting = false;
}

} } }
