#pragma once

#include "../../Tools/OneTwo.hpp"
#include "../../Tools/Int2Type.hpp"
#include "RecurserIfc.hpp"

namespace TricksAndThings { namespace detail
{

template<class T> One isRecurserInside(const typename T::Recurser *);
template<typename> Two isRecurserInside(...);

template<typename T> using IsRecurserInside = Int2Type<sizeof(isRecurserInside<T>(0)) == sizeof(One)>;

typedef Int2Type<false> NoRecurser;
typedef Int2Type<true> WithRecurser;

template<class T, class Q>
void runTaskSw(WithRecurser, T &taskPtr, Q &queuePtr)   { taskPtr->doIt(queuePtr.getRecurser()); }

template<class T, class Q>
void runTaskSw(NoRecurser, T &taskPtr, Q &)             { taskPtr->doIt(RecurserPtr(nullptr)); }

} }
