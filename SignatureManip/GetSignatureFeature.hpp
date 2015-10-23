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

#include<cstddef>
#include<type_traits>
#include "detail/SplitOffQualifiers.hpp"

namespace TricksAndThings {

template<class F, size_t inPos = 1>
class GetSignatureFeature
{   // callable object's signature analyser.
    //
    // if 'F' is something callable, this class lets you to recognize
    // F's argument type at position 'nPos' (mind that argument
    // numbering starts with 1!), it's return type, presence of
    // cv- and xvalue-ref- qualifiers for classes with defined
    // operator()(...), and the fact wether 'F' is a function or
    // a class with defined operator()(...).
    //
    // doesn't work with overloaded operator()(...) !!
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ <-- c'est la vie.
    //
    template<class Operator, size_t pos> struct GetOperatorParam;

    template<size_t pos, typename Ret, typename... Params>
    struct GetOperatorParam<Ret(F::*)(Params...), pos>
    {
        typedef typename GetSignatureFeature<Ret(Params...), pos>::Param Param;
        typedef Ret ResultOf;
    };

    typedef SplitOffQualifiers<decltype(&F::operator())> Signature;
    typedef typename Signature::Operator Operator;
    typedef GetOperatorParam<Operator, inPos> Forwarder;

    public:

    typedef typename Forwarder::Param Param;
    typedef typename Forwarder::ResultOf ResultOf;

    static const bool isConst = Signature::isConst;
    static const bool isVolatile = Signature::isVolatile;
    static const bool isLvRef = Signature::isLvRef;
    static const bool isRvRef = Signature::isRvRef;
    static const bool isFunction = Signature::isFunction;
};

template<class F, size_t pos>
struct GetSignatureFeature<F&, pos>
{
    typedef typename GetSignatureFeature<F, pos>::Param Param;
};

template<size_t pos, typename Ret, typename... Params>
struct GetSignatureFeature<Ret(*)(Params...), pos>
    : Qualifiers<false, false, false, false, true>
{
    typedef typename GetSignatureFeature<Ret(Params...), pos>::Param Param;
    typedef Ret ResultOf;
};

template<size_t pos, typename Ret, typename First, typename... Rest>
struct GetSignatureFeature<Ret(First, Rest...), pos>
{
    typedef typename GetSignatureFeature<Ret(Rest...), pos - 1>::Param Param;
};

template<typename Ret, class First, typename... Rest>
struct GetSignatureFeature<Ret(First, Rest...), 1>
{
    typedef First Param;
};

template<size_t pos, typename Ret>
struct GetSignatureFeature<Ret(), pos>
{
    typedef void Param;
};

}
