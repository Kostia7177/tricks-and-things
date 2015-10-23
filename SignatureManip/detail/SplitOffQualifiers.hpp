#pragma once

namespace TricksAndThings {

template<bool c, bool v, bool l, bool r, bool f>
struct Qualifiers
{
    static const bool isConst = c;
    static const bool isVolatile = v;
    static const bool isLvRef = l;
    static const bool isRvRef = r;
    static const bool isFunction = f;
};

template<class Operator> struct SplitOffQualifiers;

template<class F, typename R, typename... P>
struct SplitOffQualifiers<R(F::*)(P...)> :                      Qualifiers<false, false, false, false, false>
{ using Operator = R(F::*)(P...); };

template<class F, typename R, typename... P>
struct SplitOffQualifiers<R(F::*)(P...) const> :                Qualifiers<true, false, false, false, false>
{ using Operator = R(F::*)(P...); };

template<class F, typename R, typename... P>
struct SplitOffQualifiers<R(F::*)(P...) volatile> :             Qualifiers<false, true, false, false, false>
{ using Operator = R(F::*)(P...); };

template<class F, typename R, typename... P>
struct SplitOffQualifiers<R(F::*)(P...) const volatile> :       Qualifiers<true, true, false, false, false>
{ using Operator = R(F::*)(P...); };

template<class F, typename R, typename... P>
struct SplitOffQualifiers<R(F::*)(P...) &> :                    Qualifiers<false, false, true, false, false>
{ using Operator = R(F::*)(P...); };

template<class F, typename R, typename... P>
struct SplitOffQualifiers<R(F::*)(P...) const &> :              Qualifiers<true, false, true, false, false>
{ using Operator = R(F::*)(P...); };

template<class F, typename R, typename... P>
struct SplitOffQualifiers<R(F::*)(P...) volatile &> :           Qualifiers<false, true, true, false, false>
{ using Operator = R(F::*)(P...); };

template<class F, typename R, typename... P>
struct SplitOffQualifiers<R(F::*)(P...) const volatile &> :     Qualifiers<true, true, true, false, false>
{ using Operator = R(F::*)(P...); };

template<class F, typename R, typename... P>
struct SplitOffQualifiers<R(F::*)(P...) &&> :                   Qualifiers<false, false, false, true, false>
{ using Operator = R(F::*)(P...); };

template<class F, typename R, typename... P>
struct SplitOffQualifiers<R(F::*)(P...) const &&> :             Qualifiers<true, false, false, true, false>
{ using Operator = R(F::*)(P...); };

template<class F, typename R, typename... P>
struct SplitOffQualifiers<R(F::*)(P...) volatile &&> :          Qualifiers<false, true, false, true, false>
{ using Operator = R(F::*)(P...); };

template<class F, typename R, typename... P>
struct SplitOffQualifiers<R(F::*)(P...) const volatile &&> :    Qualifiers<true, true, false, true, false>
{ using Operator = R(F::*)(P...); };

}
