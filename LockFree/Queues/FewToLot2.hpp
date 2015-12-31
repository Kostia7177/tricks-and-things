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

#include "GeneralPurpose.hpp"
#include "detail/SubqueueBase.hpp"
#include "detail/ThreadSafeClientHub.hpp"
#include "Tools/RequestDmx.hpp"
#include "Tools/Traits.hpp"

namespace TricksAndThings { namespace LockFree
{
namespace detail
{

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

template<typename T, class... Params>
struct FewToLot2Wrapper
{   // we cannot declare an alias for FewToLot2 directly,
    // see http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430 ;
    typedef Lfq::RequestDmx<FewToLot2Subqueue<T,
                                              Lfq::QueueTraits
                                                <
                                                    Lfq::UseQueuePolicy<Lfq::WithSubInfoCalls, Int2Type<true>>,
                                                    Lfq::UseQueuePolicy<Lfq::WithPopWayBalancer, Int2Type<true>>,
                                                    Lfq::UseQueuePolicy<Lfq::WithPushWayBalancer, Int2Type<true>>,
                                                    Params...
                                                >>> Type;
};

} // <-- namespace detail

namespace Queues
{

template<typename... Params>
using FewToLot2 = typename detail::FewToLot2Wrapper<Params...>::Type;

}

} }
