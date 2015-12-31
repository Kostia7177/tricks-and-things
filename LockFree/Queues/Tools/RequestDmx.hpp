#pragma once
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

#include "../../detail/UsefulDefs.hpp"
#include<memory>
#include<thread>

namespace TricksAndThings { namespace LockFree
{
namespace detail
{

namespace Lfq = TricksAndThings::LockFree::Queues;

} // <-- namespace detail

namespace Queues
{

template<class Subqueue>
class RequestDmx    // request demultiplexer
{   // thing that's looking like a single queue, but in fact
    // is a wrapper for a pack of sub-queues, each of wich
    // is dedicated to it's consumer;
    // holds (and hides) this pack and demultiplexes input
    // requests ('push' calls) between the sub-queues;
    public:
    typedef typename Subqueue::Cfg Cfg;
    private:
    //
    // queue instance is to be shared by a number of consumers via
    // pointer-like proxies, so, the instance's body itself must
    // be wrapped by a shared-pointer;
    //
    struct Itself
        : Cfg::InfoCalls
    {
        Subqueue subqueues[Subqueue::Cfg::numOfConsumersLimit]; // sub-queue pack itself;
        const size_t numOfSubqueues;
        SizeAtomic numOfConsumers;
        typename Subqueue::ClientHub clientHub;

        typedef typename Cfg::MappingField MappingField;
        typedef typename Cfg::WorkloadMapCondition WorkloadMapCondition;
        typedef typename Cfg::template WorkloadMap<MappingField, WorkloadMapCondition> WorkloadMap;
        WorkloadMap workloadMap;

        template<int outOfRange, int = 0>
        struct WorkloadBalancer
        {
            static_assert(outOfRange == 0 || outOfRange == 1,
                          "\n\n\tSwitch for 'pushWayBalancer' or 'popWayBalancer'\n\t"
                          "cannot be other than 'false' or 'true'!\n");
        };

        template<int unused>
        struct WorkloadBalancer<0, unused>
        {
            WorkloadBalancer(Itself *){}
            template<typename T> void tryFix(bool &, T &){}
            bool get(size_t *){ return false; }
            void put(size_t){}
        };

        template<int unused>
        class WorkloadBalancer<1, unused>
        {
            Itself *subj;
            Subqueue *subqueues;

            public:

            WorkloadBalancer(Itself *s)
                : subj(s), subqueues(subj->subqueues){}

            template<typename T> void tryFix(bool &fetched, T &p)
            {
                size_t idx;
                // first, look, wether there is any queue left by
                // it's consumer
                if (subj->exitedConsumersMap.getLowest(&idx, [&](size_t i)
                                                             { return !subj->subqueues[i].empty(); }))
                {
                    fetched = subj->subqueues[idx].pop(p);
                }

                if (!fetched
                    && subj->workloadMap.eject0If(&idx,
                                                  subqueues,
                                                  subj->numOfSubqueues))
                {
                    fetched = subj->subqueues[idx].pop(p);
                    subj->pushWayBalancer.put(idx);
                }
            }

            bool get(size_t *idxRet)
            {
                do
                {
                    if (!subj->workloadMap.ejectIf(idxRet,
                                                   subqueues,
                                                   subj->numOfSubqueues))
                    { return false; }
                }
                while (subj->exitedConsumersMap.contains(*idxRet));

                return true;
            }

            void put(size_t num)
            { subj->workloadMap.injectIf(num,
                                         subqueues,
                                         subj->numOfSubqueues); }
        };

        WorkloadBalancer<Cfg::pushWayBalancer> pushWayBalancer;
        WorkloadBalancer<Cfg::popWayBalancer> popWayBalancer;

        typename Cfg::template ExitedConsumersMap<MappingField> exitedConsumersMap;

        Subqueue *selectSubqueue(size_t *); // multiplexes input ('push') requests;
        Subqueue *getSubqueue();            // acquires the sub-queue when a consumer
                                            // is initializing;
        template<typename... Args>
        Itself(size_t, Args &&...);

        void incrSize()             { Subqueue::Cfg::InfoCalls::incrSize(); }
        void decrSize()             { Subqueue::Cfg::InfoCalls::decrSize(); }
        size_t subSize(size_t idx)  { return subqueues[idx]->size(); }
    };

    typedef std::shared_ptr<Itself> QueuePtr;
    QueuePtr itself;

    public:

    RequestDmx() : itself(new Itself(0)){}

    template<typename... Args>
    RequestDmx(size_t s, Args &&... args)
        : itself(new Itself(s, std::forward<Args>(args)...)){}

    RequestDmx(const RequestDmx &) = delete;
    RequestDmx(RequestDmx &&q) : itself(std::move(q)){}

    RequestDmx &operator=(RequestDmx &&);
    RequestDmx &operator=(const RequestDmx &) = delete;

    size_t size() const { return itself->size(); }

    template<typename... Args>
    void setWorkloadMapCondition(Args &&... args)
    { itself->workloadMap.setCondition(std::forward<Args>(args)...); }

    typedef typename Subqueue::Type Type;
    typedef typename Cfg::ConsumerIdle ConsumerIdle;

    // both pop and push are available through
    // the smart pointer-like proxies only;

    class ConsumerSideProxy
    {
        QueuePtr queue;
        protected:
        Subqueue *subqueue;
        private:
        size_t subqueueIdx;
        std::thread::id threadId;
        public:
        ConsumerSideProxy(RequestDmx *);
        ~ConsumerSideProxy();
        ConsumerSideProxy *operator->() { return this; }
        size_t subSize() const          { return subqueue->size(); }
        bool pop(Type &);
    };

    class ProviderSideProxy
    {
        QueuePtr queue;
        size_t idx;
        public:
        ProviderSideProxy(RequestDmx *);
        ~ProviderSideProxy()
        { queue->clientHub.onProviderExited(); }
        ProviderSideProxy *operator->() { return this; }
        void push(Type &&);
        template<class F>
        void apply(F f) { f(idx); }
    };
};

} } }
#include "../detail/RequestDmx.tcc"
