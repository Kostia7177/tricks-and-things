#pragma once

#include "CfgComponents.hpp"
#include "../../EasyTraits/EasyTraits.hpp"
#include "../Tools/DefaultAllocatingStorage.hpp"
#include "Tools/InfoCalls.hpp"
#include "../../Tools/Int2Type.hpp"
#include "../../Tools/Template2Type.hpp"
#include "../../Tools/NullType.hpp"
#include "../Tools/BinaryMapper.hpp"
#include "detail/ConsumerIdle.hpp"
#include "detail/SubqueueIsEmpty.hpp"

namespace TricksAndThings { namespace LockFree { namespace Queues
{

typedef DefaultSettingsBox
    <
        StorageIs<Template2Type<DefaultAllocatingStorage>>,
        InfoCallsAre<Template2Type<NoInfoCalls>>,
        SubInfoCallsAre<Template2Type<NoInfoCalls>>,
        PageSizeIs<Int2Type<4096>>,
        NumOfConsumersLimitIs<Int2Type<128>>,
        WorkloadMapIs<Template2Type<BinaryMapperCond>>,
        ExitedConsumersMapIs<Template2Type<BinaryMapper>>,
        MappingFieldIs<uint64_t>,
        WorkloadMapConditionIs<ContainerIsNearEmpty<>>,
        PushWayBalancerIs<Int2Type<false>>,
        PopWayBalancerIs<Int2Type<false>>,
        ConsumerIdleIs<TricksAndThings::detail::ConsumerIdle>
    > DefaultSettings;

template<class... Params>
using QueueTraits =
    EasyTraits<DefaultSettings, 0, Params...>;

template<template<class, class> class PolicyWrapper, class Policy>
using UseQueuePolicy =
    ReplaceDefaultSettingWithPolicy<DefaultSettings, PolicyWrapper, Policy>;

} } }
