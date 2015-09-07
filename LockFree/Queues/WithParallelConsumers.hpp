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
#include<boost/mpl/map.hpp>
#include<boost/mpl/at.hpp>
#include<thread>
#include<mutex> // never mind, it's really still lock-free,
#include<condition_variable>    // yeah, don't worry;

namespace TricksAndThings { namespace LockFree
{

namespace detail
{
enum { passBy, doLookup };
}

namespace Queues {

enum PushWayBalancer
{
    passByPushWayBalancer = detail::passBy,
    pushWayLookup = detail::doLookup
};

enum PopWayBalancer
{
    passByPopWayBalancer = detail::passBy,
    popWayLookup = detail::doLookup
};

namespace Bm = boost::mpl;
typedef uint64_t MappingField;
template<class Subqueue,
         PushWayBalancer pushWayBalancerIdx = passByPushWayBalancer,
         PopWayBalancer popWayBalancerIdx = passByPopWayBalancer,
         class WorkloadMap = BinaryMapper<MappingField, ContainerIsNearEmpty<Subqueue>>>
class WithParallelConsumers
    : public Subqueue::Cfg::InfoCalls
{   // wrapper for a pack of sub-queues, each of wich is
    // dedicated to it's consumer;
    // holds (and hides) this pack and multiplexes input
    // requests ('push') calls between the sub-queues;
    //
    Subqueue subqueues[Subqueue::Cfg::numOfConsumersLimit]; // sub-queue pack itself;
    SizeAtomic numOfConsumers;
    typename Subqueue::ClientHub clientHub;
    WorkloadMap workloadMap;
    BinaryMapper<MappingField> exitedConsumersMap;

    struct PassBy
    {
        PassBy(WithParallelConsumers *){}
        template<typename T> void tryFix(bool &, T &){}
        bool pop(size_t *){ return false; }
        void push(size_t){}
    };
    class DoLookup
    {
        WithParallelConsumers *subj;
        public:
        DoLookup(WithParallelConsumers *s) : subj(s){}
        template<typename T> void tryFix(bool &, T &);
        bool pop(size_t *);
        void push(size_t num)
        { subj->workloadMap.push(num); }
    };

    typedef typename Bm::map
        <
            Bm::pair<Bm::int_<detail::passBy>, PassBy>,
            Bm::pair<Bm::int_<detail::doLookup>, DoLookup>
        >::type Balancers;

    typename boost::mpl::at<Balancers, Bm::int_<pushWayBalancerIdx>>::type pushWayBalancer;
    typename boost::mpl::at<Balancers, Bm::int_<popWayBalancerIdx>>::type popWayBalancer;

    Subqueue *selectSubqueue(size_t *); // multiplexes input ('push') requests;
    Subqueue *getSubqueue();            // acquires the sub-queue when a consumer
                                        // is initializing;

    public:

    WithParallelConsumers(size_t = 0);

    typedef typename Subqueue::Type Type;

    // both pop and push are available through
    // the smart pointer-like proxies only;

    class ConsumerSideProxy
    {
        WithParallelConsumers *queue;
        Subqueue *subqueue;
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
        WithParallelConsumers *queue;
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
