#pragma once

#include "../Tools/NullType.hpp"

namespace TricksAndThings
{

template<class S, class Base = NullType>
struct StatisticsAre : virtual Base
{ typedef S Statistics; };

template<class Q, class Base = NullType>
struct QueueIs : virtual Base
{ template<typename... T>
  using Queue = typename Q::template Template<T...>; };

template<class P, class Base = NullType>
struct ShutdownPolicyIs : virtual Base
{ enum { shutdownPolicy = P::value }; };

template<class W, class Base = NullType>
struct WorkerIs : virtual Base
{ template<typename... T>
  using Worker = typename W::template Template<T...>; };

template<class M, class Base = NullType>
struct ManagerIs : virtual Base
{ template<typename... T>
  using Manager = typename M::template Template<T...>; };

template<class C, class Base = NullType>
struct WorkerConditionIs : virtual Base
{ template<class W>
  using WorkerCondition = typename C::template Template<W>; };

}
