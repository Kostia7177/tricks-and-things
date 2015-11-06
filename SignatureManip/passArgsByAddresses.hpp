#pragma once
/*
    Their Gorokhov's Majesty request...

    Copyright (C) 2015 Konstantin U. Zozoulia

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

#include<utility>

namespace TricksAndThings {

enum RvalueRefs
{
    disallowRvalueRefs = false,
    allowRvalueRefs = true
};

namespace detail {

template<RvalueRefs, typename T>
T *convertor(T *p)  { return p; }   // the pointer is to be relayed as is;

template<RvalueRefs, typename T>
T *convertor(T &r)  { return &r; }  // taking the address if it's a reference;

template<RvalueRefs rvalueRefsAllowed, typename T>
T *convertor(T &&p) // if rvalue-refs aren't allowed, break the build;
{
    struct Inaccessible {};
    static_assert(rvalueRefsAllowed || std::is_same<T, Inaccessible>::value,
                  "\n\n\tConverting of an rvalue-reference to an address"
                  "\n\tis not allowed - 'passArgsByAddress' takes"
                  "\n\teither addresses themselves or lvalue-references!\n");

    return &p;
}

}   // <-- namespace 'detail'

template<typename Ret, RvalueRefs rvalueRefsSwitch = disallowRvalueRefs,
         class F, typename... Args>
Ret passArgsByAddresses(F f, Args &&... args)   // 'f' is a fuction that is to be called;
{ return f(detail::convertor<rvalueRefsSwitch>(std::forward<Args>(args))...); }

}
