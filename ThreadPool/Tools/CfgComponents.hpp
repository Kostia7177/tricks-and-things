#pragma once

#include "../../Tools/NullType.hpp"

namespace TricksAndThings
{

template<class S, class Base = NullType>
struct StatisticsAre : virtual Base
{ typedef S Statistics; };

template<class Base>
struct StatisticsAre<NullType, Base> : virtual Base
{
    struct Statistics
    {
        template<class Queue> void store(Queue &){}
        void clear(){}
        void stopped(){}
    };
};

template<class Base>
inline static std::ostream &operator<<(std::ostream &s, const StatisticsAre<NullType, Base> &)
{ return s; }

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

}
