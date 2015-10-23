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
#include <type_traits>
#include "../ParamPackManip/Params2Hierarchy.hpp"
#include "../ParamPackManip/Hierarchy2Params.hpp"
#include "detail/Binders/BindNotNullsOnly.hpp"

namespace TricksAndThings
{

template<class F, typename... Params>
int filteringAdapter(F f, Params &... params)
{   // -- aim:
    //
    // suppose there is a fixed-size set of variable types:
    //
    //  t1       t2       t3       t4       t5
    //
    // that are defined inside any traits structure. any of theese types
    // can be instantiated by an 'empty' NullType, and any others - by
    // some meaningfull Ti:
    //
    //  T1       NullType T3       T4       NullType    -- (ex 1)
    //  NullType NullType NullType NullType T5          -- (ex 2)
    //  T1       T2       NullType T4       NullType    -- (ex 3)
    //  NullType T2       NullType NullType T5          -- (ex 4)
    //
    // suppose now, that we want to pass the objects of theese types to any client's
    // function, which knows nothing about our 'NullType' (and, worst of all, don't
    // want to know).
    // for our four cases of theese client's functions are:
    //
    //  int f1(T1, T3, T4);     // ex 1
    //  int f2(T5);             // ex 2
    //  int f3(T1, T2, T4);     // ex 3
    //  int f4(T2, T5);         // ex 4
    //
    // so, all we've got to do is to filter out all the NullType-objects and to pass
    // the filtered set to a client's code.
    //
    // -- implementation:
    //
    // 'Params...' contains the input (dirty) set of types;
    typedef Params2Hierarchy<BindNotNullsOnly, Params...> Hierarchy;
    Hierarchy h(params...);  // simply creating a 'filtered hierarchy'...
    // ...and simply building a new, filtered, variadic. And then simply call a payload,
    return Hierarchy2Params<Hierarchy>::call(f, h);
    // ...and nothing more.
}
}
