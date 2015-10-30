#pragma once

#include "../../ParamPackManip/Params2TypesHierarchy.hpp"
#include "../../EasyTraits/EasyTraits.hpp"
#include "../Tools/DefaultAllocatingStorage.hpp"
#include "Tools/InfoCalls.hpp"
#include "../../Tools/Int2Type.hpp"
#include "../../Tools/NullType.hpp"
#include "../Tools/BinaryMapper.hpp"

namespace TricksAndThings { namespace LockFree { namespace Queues
{

template<template<typename> class S, class Base = NullType>
struct StorageIs : virtual Base
{ template<typename T> using Storage = S<T>; };

template<template<typename> class C, class Base = NullType>
struct InfoCallsAre : virtual Base
{ typedef C<NullType> InfoCalls; };

template<template<typename> class SubC, class Base = NullType>
struct SubInfoCallsAre : virtual Base
{ typedef SubC<NullType> SubInfoCalls; };

template<class S, class Base = NullType>
struct SizeIs : virtual Base
{ static const size_t pageSize = S::value; };

template<class N, class Base = NullType>
struct NumOfConsumersLimitIs : virtual Base
{ static const size_t numOfConsumersLimit = N::value; };

template<template<typename> class Map, class Base = NullType>
struct WorkloadMapIs : virtual Base
{ template<class Condition> using WorkloadMap = Map<Condition>; };

template<class Condition> using BinaryMapperCond64Bit = BinaryMapperCond<MappingField, Condition>;

template<template<typename> class MappingCondition, class Base = NullType>
struct WorkloadMapConditionIs : virtual Base
{ template<class Subqueue> using WorkloadMapCondition = MappingCondition<Subqueue>; };

template<class N, class Base = NullType>
struct PushWayBalancerIs : virtual Base
{ enum { pushWayBalancer = N::value }; };

template<class N, class Base = NullType>
struct PopWayBalancerIs : virtual Base
{ enum { popWayBalancer = N::value }; };

typedef Params2TypesHierarchy
    <
        StorageIs<DefaultAllocatingStorage>,
        InfoCallsAre<Components::NoInfoCalls>,
        SubInfoCallsAre<Components::NoInfoCalls>,
        SizeIs<Int2Type<4096>>,
        NumOfConsumersLimitIs<Int2Type<128>>,
        WorkloadMapIs<BinaryMapperCond64Bit>,
        WorkloadMapConditionIs<ContainerIsNearEmpty>,
        PushWayBalancerIs<Int2Type<false>>,
        PopWayBalancerIs<Int2Type<false>>
    > DefaultSettings;
template<class... Params>
using Traits = EasyTraits<DefaultSettings, 0, Params...>;
template<template<class, class> class PolicyWrapper, class Policy>
using UsePolicy = ReplaceDefaultSettingWithPolicy<DefaultSettings, PolicyWrapper, Policy>;
template<template<template<typename> class, class> class PolicyWrapper, template<typename> class Policy>
using UsePolicyTemplate = ReplaceDefaultSettingWithPolicyTemplate<DefaultSettings, PolicyWrapper, Policy>;

} } }
