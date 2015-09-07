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

#include<atomic>
#include<cstddef>
#include<functional>
#include<math.h>
#include "../../Tools/NullType.hpp"

namespace TricksAndThings { namespace LockFree {

template<class Container>
class ContainerIsNearEmpty
{
    Container *containers;
    size_t treshold;

    public:

    ContainerIsNearEmpty(Container *c, size_t t = 1)
        : treshold(t), containers(c){}

    bool operator()(size_t idx) const
    { return containers[idx].size() <= treshold; }
};

template<class BitMap> BitMap int2ShiftedBit(size_t);

template<class BitMap> size_t shiftedBit2Int(BitMap bit)
{ return log2(bit); }

template<class BitMap> BitMap lowestBit(BitMap);

template<class BitMap, class Condition = NullType>
class BinaryMapper
{
    std::atomic<BitMap> bitMap;
    Condition condition;

    template<typename... Args>
    static bool matches(const NullType &, bool isPush, size_t, Args &&...)
    { return isPush; }

    template<class C, typename... Args>
    static bool matches(const C &c, bool, size_t idx, Args &&... args)
    { return c(idx, std::forward<Args>(args)...); }

    template<typename... Args>
    bool lambdaAtPop(size_t *, BitMap &, Args &&...);

    template<typename... Args>
    bool lambdaAtPush(size_t, BitMap &, Args &&...);

    public:

    BinaryMapper(Condition c = Condition())
        : bitMap(0), condition(c){}

    template<class... Args>
    void push(size_t, Args &&...);

    template<class... Args>
    bool pop(size_t *, Args &&...);

    void erase(size_t num)
    { bitMap.fetch_and(~int2ShiftedBit<BitMap>(num)); }

    bool getLowest1(size_t *, bool = false);

    bool getLowest0(size_t *p)
    { return getLowest1(p, true); }

    bool contains(size_t num)
    { return bitMap.load() & int2ShiftedBit<BitMap>(num); }
};

} }
#include "BinaryMapper.tcc"
