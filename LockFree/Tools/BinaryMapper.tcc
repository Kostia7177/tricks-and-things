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

#include<sstream>
#include<stdexcept>
#include "../detail/casIf.hpp"
#include "../../Tools/GccBug47226Satellite.hpp"

namespace TricksAndThings { namespace LockFree {

template<class BitMap>
BitMap int2ShiftedBit(size_t num)
{
    size_t sizeOfBitMap = sizeof(BitMap) * 8;
    if (num > sizeOfBitMap)
    {
        std::stringstream exc;
        exc << "Number "
            << num << " cannot be mapped 'cos it is out "
                      "of range, it must be less than "
            << sizeOfBitMap << ";";
        throw std::runtime_error(exc.str());
    }

    return pow(2, num);
}

template<class BitMap>
BitMap lowestBit(BitMap value)
{
    BitMap retBit = value - 1;
    retBit ^= value;
    retBit &= value;

    return retBit;
}

template<class BitMap, class Condition>
template<typename... Args>
bool BinaryMapper<BitMap, Condition>::lambdaAtPop(
    size_t *ret,
    BitMap &value,
    Args &&... args)
{
    GccBug47226Satellite();
    if (bitMap == 0)
    { return false; }

    BitMap retBit = lowestBit(value);

    *ret = shiftedBit2Int(retBit);

    if (!matches(condition, false, *ret, std::forward<Args>(args)...))
    {
        value &= ~retBit;
    }

    return true;
}

template<class BitMap, class Condition>
template<typename... Args>
bool BinaryMapper<BitMap, Condition>::lambdaAtPush(
    size_t num,
    BitMap &value,
    Args &&... args)
{
    GccBug47226Satellite();
    if (!matches(condition, true, num, std::forward<Args>(args)...)) { return false; }

    value |= int2ShiftedBit<BitMap>(num);

    return true;
}

template<class BitMap, class Condition>
template<class... Args>
void BinaryMapper<BitMap, Condition>::push(
    size_t num,
    Args &&... args)
{
    detail::casIf<>(bitMap,
                    std::bind(&BinaryMapper::lambdaAtPush<Args...>,
                              this,
                              num,
                              std::placeholders::_1,
                              std::forward<Args>(args)...));
}

template<class BitMap, class Condition>
template<class... Args>
bool BinaryMapper<BitMap, Condition>::pop(
    size_t *ret,
    Args &&... args)
{
    return detail::casIf<>(bitMap,
                           std::bind(&BinaryMapper::lambdaAtPop<Args...>,
                                     this,
                                     ret,
                                     std::placeholders::_1,
                                     std::forward<Args>(args)...));
}

template<class BitMap, class Condition>
bool BinaryMapper<BitMap, Condition>::getLowest1(
    size_t *ret,
    bool invert)
{
    BitMap retBit = bitMap.load();
    if (invert) { retBit = ~retBit; }

    if (retBit)
    {
        retBit = lowestBit(retBit);
        *ret = shiftedBit2Int(retBit);
        return true;
    }

    return false;
}

} }
