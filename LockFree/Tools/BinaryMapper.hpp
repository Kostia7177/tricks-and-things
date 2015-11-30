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

#include "../../Tools/NullType.hpp"
#include "../detail/casIf.hpp"
#include<atomic>
#include<cstddef>
#include<functional>
#include<math.h>

namespace TricksAndThings { namespace LockFree
{

template<size_t t = 1>
struct ContainerIsNearEmpty
{
    static const size_t treshold = t;

    template<typename... Args>
    ContainerIsNearEmpty(Args &&...){}

    template<class Container, typename... Args>
    bool operator()(
        size_t idx,
        const Container *containers,
        Args &&...) const
    { return containers[idx].size() <= treshold; }
};

template<class BitMap> BitMap int2ShiftedBit(size_t);

template<class BitMap> size_t shiftedBit2Int(BitMap bit)
{ return log2(bit); }

template<class BitMap> BitMap lowestBit(BitMap);

template<class BitMap, class C, typename... Args>
bool ejectBit(size_t *, BitMap &, const C &, Args &&...);

template<class BitMap>
class BinaryMapper
{
    std::atomic<BitMap> bitMap;

    public:

    BinaryMapper() : bitMap(0){}

    void inject(size_t);
    bool eject(size_t *);

    template<class C>
    bool getLowest(size_t *, const C &);

    bool contains(size_t num)
    { return bitMap.load() & int2ShiftedBit<BitMap>(num); }

    void erase(size_t num)
    { bitMap.fetch_and(~int2ShiftedBit<BitMap>(num)); }

    template<class F>
    bool applyIf(F f)
    { return detail::casIf<>(bitMap, f); }
};

template<class BitMap, class Condition>
class BinaryMapperCond
{
    BinaryMapper<BitMap> itself;
    Condition condition;

    template<typename... Args>
    bool lambdaAtPop0(size_t *, BitMap &, Args &&...);

    template<typename... Args>
    bool lambdaAtPush(size_t, BitMap &, Args &&...);

    public:

    BinaryMapperCond(){}

    template<typename... Args>
    BinaryMapperCond(Args &&... args)
        : condition(std::forward<Args>(args)...){}

    template<typename... Args>
    void setCondition(Args &&... args)
    { condition = Condition(std::forward<Args>(args)...); }

    template<class... Args>
    void injectIf(size_t, Args &&...);

    template<class... Args>
    bool ejectIf(size_t *, Args &&...);

    template<typename... Args>
    bool eject0If(size_t *p, Args &&...);

    void erase(size_t num)
    { itself.erase(num); }
};

} }
#include "BinaryMapper.tcc"
