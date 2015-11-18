#pragma once

#include<cstddef>

namespace TricksAndThings { namespace LockFree { namespace Queues
{

template<class S, class Base = NullType>
struct StorageIs : virtual Base
{ template<typename T>
  using Storage = typename S::template Template<T>; };

template<class C, class Base = NullType>
struct InfoCallsAre : virtual Base
{ typedef typename C::template Template<NullType> InfoCalls; };

template<class SubC, class Base = NullType>
struct SubInfoCallsAre : virtual Base
{ typedef typename SubC::template Template<NullType> SubInfoCalls; };

template<class S, class Base = NullType>
struct PageSizeIs : virtual Base
{ static const size_t pageSize = S::value; };

template<class N, class Base = NullType>
struct NumOfConsumersLimitIs : virtual Base
{ static const size_t numOfConsumersLimit = N::value; };

template<class M, class Base = NullType>
struct WorkloadMapIs : virtual Base
{ template<typename Field, class Condition>
  using WorkloadMap =
      typename M::template Template<Field, Condition>; };

template<class F, class Base = NullType>
struct MappingFieldIs : virtual Base
{ typedef F MappingField; };

template<class MappingCondition, class Base = NullType>
struct WorkloadMapConditionIs : virtual Base
{ template<class Subqueue>
  using WorkloadMapCondition =
      typename MappingCondition::template Template<Subqueue>; };

template<class N, class Base = NullType>
struct PushWayBalancerIs : virtual Base
{ enum { pushWayBalancer = N::value }; };

template<class N, class Base = NullType>
struct PopWayBalancerIs : virtual Base
{ enum { popWayBalancer = N::value }; };

template<class C, class Base = NullType>
struct ConsumerIdleIs : virtual Base
{ typedef C ConsumerIdle; };

} } }
