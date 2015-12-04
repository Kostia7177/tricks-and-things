#pragma once

#include "../../Tools/OneTwo.hpp"
#include "../../Tools/Int2Type.hpp"
#include<cstddef>

namespace TricksAndThings { namespace LockFree { namespace detail
{

template<class F> One withRangeCheckerInside(const typename F::RangeChecker *);
template<class F> Two withRangeCheckerInside(...);

typedef Int2Type<true> WithRangeChecker;
typedef Int2Type<false> WithoutRangeChecker;

template<class F>
bool checkRange(const F &f, size_t n, const WithRangeChecker &)
{ return f.checkRange(n); }

template<class F>
bool checkRange(const F &f, size_t n, const WithoutRangeChecker &)
{ return true; }

} } }
