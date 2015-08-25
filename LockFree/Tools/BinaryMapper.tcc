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

namespace TricksAndThings { namespace LockFree {

template<class BitMap, class Condition>
BitMap BinaryMapper<BitMap, Condition>::int2ShiftedBit(size_t num)
{
    if (num > sizeof(BitMap) * 8)
    {
        std::stringstream exc;
        exc << "Number "
            << num << " cannot be mapped 'cos it is out "
                      "of range, it must be less than "
            << sizeof(BitMap) * 8 << ";";
        throw std::runtime_error(exc.str());
    }

    BitMap newBit = 0x1;
    while (num)
    {
        -- num;
        newBit <<= 1;
    }
    return newBit;
}

template<class BitMap, class Condition>
template<class... Args>
void BinaryMapper<BitMap, Condition>::push(
    size_t num,
    Args &&... args)
{
    if (!condition(std::forward<Args>(args)...)) { return; }

    bitMap.fetch_or(int2ShiftedBit(num));
}

template<class BitMap, class Condition>
bool BinaryMapper<BitMap, Condition>::pop(size_t *ret)
{
    BitMap retBit;
    bool nothing = !detail::casIf<>(bitMap, [&](BitMap &value)
                                            {
                                                if (bitMap == 0)
                                                { return false; }

                                                retBit = value - 1;
                                                retBit ^= value;
                                                retBit &= value;
                                                value &= ~retBit;
                                                return true;
                                            });
    if (nothing) { return false; }

    int num = 0;
    while (retBit != 1)
    {
        ++ num;
        retBit >>= 1;
    }
    *ret = num;

    return true;
}

} }
