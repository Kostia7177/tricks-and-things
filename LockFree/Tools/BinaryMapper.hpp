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
#include "../detail/AlwaysYes.hpp"

namespace TricksAndThings { namespace LockFree {

class ContainerIsNearEmpty
{
    size_t treshold;
    public:
    ContainerIsNearEmpty(size_t t = 1) : treshold(t) {}
    template<class C>
    bool operator()(const C &container)
    { return container.size() <= treshold; }
};

template<class BitMap, class Condition = TricksAndThings::detail::AlwaysYes>
class BinaryMapper
{
    std::atomic<BitMap> bitMap;
    Condition condition;
    BitMap int2ShiftedBit(size_t);

    public:

    BinaryMapper(Condition c = Condition())
        : bitMap(0), condition(c){}

    template<class... Args>
    void push(size_t, Args &&...);

    bool pop(size_t *);

    bool contains(size_t num)
    { return bitMap.load() & int2ShiftedBit(num); }
};

} }
#include "BinaryMapper.tcc"
