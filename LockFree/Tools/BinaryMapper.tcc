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
#include "../detail/withRangeChecker.hpp"
#include "../../Tools/GccBug47226Satellite.hpp"

namespace TricksAndThings { namespace LockFree
{

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

template<class BitMap, class C, typename... Args>
bool ejectBit(
    size_t *ret,
    BitMap &value,
    const C &c,
    Args &&... args)
{
    while (value != 0)
    {
        BitMap retBit = lowestBit(value);
        size_t num = shiftedBit2Int(retBit);
        if (!detail::checkRange(c,
                                num,
                                Int2Type<sizeof(detail::withRangeCheckerInside<C>(0))
                                         == sizeof(One)>()))
        { return false; }

        if (c(num, std::forward<Args>(args)...))
        {
            *ret = num;
            return true;
        }
        value &= ~retBit;
    }

    return false;
}

template<class BitMap>
void BinaryMapper<BitMap>::inject(size_t num)
{
    BitMap newBit = int2ShiftedBit<BitMap>(num);
    detail::casIf<>(bitMap, [=](BitMap &value)
                            { return value |= newBit, true; });
}

template<class BitMap>
bool BinaryMapper<BitMap>::eject(size_t *ret)
{
    return detail::casIf<>(bitMap, [=](BitMap &value)
                                   {
                                    if (value == 0) { return false; }
                                    BitMap retBit = lowestBit(value);
                                    *ret = shiftedBit2Int(retBit);
                                    value &= ~retBit;
                                    return true;
                                   });
}

template<class BitMap>
template<class C>
bool BinaryMapper<BitMap>::getLowest(
    size_t *ret,
    const C &c)
{
    BitMap bmCopy = bitMap.load();

    return ejectBit(ret, bmCopy, c);
}

template<class BitMap, class Condition>
template<typename... Args>
bool BinaryMapperCond<BitMap, Condition>::lambdaAtPop0(
    size_t *ret,
    BitMap &value,
    Args &&... args)
{
    GccBug47226Satellite();
    for (BitMap inverted; (inverted = ~value) != 0; )
    {
        BitMap retBit = lowestBit(inverted);
        size_t num = shiftedBit2Int(retBit);
        if (!detail::checkRange(condition,
                                num,
                                Int2Type<sizeof(detail::withRangeCheckerInside<Condition>(0))
                                         == sizeof(One)>()))
        { return false; }

        if (!condition(num, std::forward<Args>(args)...))
        {
            *ret = num;
            return true;
        }
        else { value |= retBit; }
    }

    return false;
}

template<class BitMap, class Condition>
template<typename... Args>
bool BinaryMapperCond<BitMap, Condition>::lambdaAtPush(
    size_t num,
    BitMap &value,
    Args &&... args)
{
    GccBug47226Satellite();
    if (!detail::checkRange(condition,
                            num,
                            Int2Type<sizeof(detail::withRangeCheckerInside<Condition>(0))
                                     == sizeof(One)>())
        || !condition(num, std::forward<Args>(args)...))
    { return false; }

    value |= int2ShiftedBit<BitMap>(num);

    return true;
}

template<class BitMap, class Condition>
template<class... Args>
void BinaryMapperCond<BitMap, Condition>::injectIf(
    size_t num,
    Args &&... args)
{
    itself.applyIf(std::bind(&BinaryMapperCond::lambdaAtPush<Args...>,
                             this,
                             num,
                             std::placeholders::_1,
                             std::forward<Args>(args)...));
}

template<class BitMap, class Condition>
template<class... Args>
bool BinaryMapperCond<BitMap, Condition>::ejectIf(
    size_t *ret,
    Args &&... args)
{
    GccBug47226Satellite(); // replace bind with lambda;
    return itself.applyIf(std::bind(&ejectBit<BitMap, Condition, Args...>,
                                    ret,
                                    std::placeholders::_1,
                                    condition,
                                    std::forward<Args>(args)...));
}

template<class BitMap, class Condition>
template<typename... Args>
bool BinaryMapperCond<BitMap, Condition>::eject0If(
    size_t *ret,
    Args &&... args)
{
    return itself.applyIf(std::bind(&BinaryMapperCond::lambdaAtPop0<Args...>,
                                    this,
                                    ret,
                                    std::placeholders::_1,
                                    std::forward<Args>(args)...));
}

} }
