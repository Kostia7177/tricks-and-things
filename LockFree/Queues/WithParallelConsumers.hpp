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

#include "../detail/UsefulDefs.hpp"
#include "../Tools/BinaryMapper.hpp"
#include "BalancerIndexes.hpp"
#include<boost/mpl/map.hpp>
#include<boost/mpl/at.hpp>
#include<thread>
#include<mutex> // never mind, it's really still lock-free,
#include<condition_variable>    // yeah, don't worry;

namespace TricksAndThings { namespace LockFree
{
namespace Queues {

namespace Bm = boost::mpl;

template<class Subqueue>
class WithParallelConsumers
{   // wrapper for a pack of sub-queues, each of wich is
    // dedicated to it's consumer;
    // holds (and hides) this pack and multiplexes input
    // requests ('push') calls between the sub-queues;
    typedef typename Subqueue::Cfg Cfg;
    //
    // queue instance is to be shared by a number of consumers via
    // pointer-like proxies, so, the instance's body itself must
    // be wrapped by a shared-pointer;
    //
    struct Itself
        : Cfg::InfoCalls
    {
        Subqueue subqueues[Subqueue::Cfg::numOfConsumersLimit]; // sub-queue pack itself;
        SizeAtomic numOfConsumers;
        typename Subqueue::ClientHub clientHub;

        typedef typename Cfg::template WorkloadMapCondition<Subqueue> WorkloadMapCondition;
        typedef typename Cfg::template WorkloadMap<WorkloadMapCondition> WorkloadMap;
        WorkloadMap workloadMap;

        BinaryMapper<MappingField> exitedConsumersMap;

        struct PassBy
        {
            PassBy(Itself *){}
            template<typename T> void tryFix(bool &, T &){}
            bool get(size_t *){ return false; }
            void put(size_t){}
        };
        class DoLookup
        {
            Itself *subj;
            public:
            DoLookup(Itself *s) : subj(s){}
            template<typename T> void tryFix(bool &, T &);
            bool get(size_t *);
            void put(size_t num)
            { subj->workloadMap.injectIf(num); }
        };

        typedef typename Bm::map
            <
                Bm::pair<Bm::int_<detail::passBy>, PassBy>,
                Bm::pair<Bm::int_<detail::doLookup>, DoLookup>
            >::type Balancers;

        typename boost::mpl::at<Balancers, Bm::int_<Cfg::pushWayBalancerIdx>>::type pushWayBalancer;
        typename boost::mpl::at<Balancers, Bm::int_<Cfg::popWayBalancerIdx>>::type popWayBalancer;

        Subqueue *selectSubqueue(size_t *); // multiplexes input ('push') requests;
        Subqueue *getSubqueue();            // acquires the sub-queue when a consumer
                                            // is initializing;
        template<typename... Args>
        Itself(size_t, Args &&...);
        void incrSize(){ Subqueue::Cfg::InfoCalls::incrSize(); }
        void decrSize(){ Subqueue::Cfg::InfoCalls::decrSize(); }
        size_t subSize(size_t idx){ return subqueues[idx]->size(); }
    };

    typedef std::shared_ptr<Itself> QueuePtr;
    QueuePtr itself;

    public:

    WithParallelConsumers() : itself(new Itself(0)){}
    template<typename... Args>
    WithParallelConsumers(size_t s, Args &&... args)
        : itself(new Itself(s, std::forward<Args>(args)...)){}
    WithParallelConsumers(const WithParallelConsumers &) = delete;
    WithParallelConsumers(WithParallelConsumers &&q) : itself(std::move(q)){}

    WithParallelConsumers &operator=(WithParallelConsumers &&);
    WithParallelConsumers &operator=(const WithParallelConsumers &) = delete;

    size_t size(){ return itself->size(); }
    size_t subSize(size_t idx){ return itself->size(); }

    typedef typename Subqueue::Type Type;

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
        ConsumerSideProxy(WithParallelConsumers *);
        ~ConsumerSideProxy();
        ConsumerSideProxy *operator->() { return this; }
        size_t subSize(){ return subqueue->size(); }
        bool pop(Type &);
    };

    class ProviderSideProxy
    {
        QueuePtr queue;
        size_t idx;
        public:
        ProviderSideProxy(WithParallelConsumers *);
        ~ProviderSideProxy()
        { queue->clientHub.onProviderExited(); }
        ProviderSideProxy *operator->() { return this; }
        void push(Type &&);
        template<class F>
        void apply(F f) { f(idx); }
    };

    class ConsumerIdle
    {
        std::mutex lock;
        std::condition_variable check;
        std::atomic<bool> awaiting;
        public:
        ConsumerIdle() : awaiting(false){}
        template<class F> void until(F f);
        void kick(){ check.notify_one(); }
        void interrupt()
        { if (awaiting) { kick(); } }
    };
};

} } }
#include "WithParallelConsumers.tcc"
