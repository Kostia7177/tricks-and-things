#pragma once

#include "CfgComponents.hpp"
#include "../../EasyTraits/EasyTraits.hpp"
#include "../Tools/DefaultAllocatingStorage.hpp"
#include "Tools/InfoCalls.hpp"
#include "../../Tools/Int2Type.hpp"
#include "../../Tools/Template2Type.hpp"
#include "../../Tools/NullType.hpp"
#include "../Tools/BinaryMapper.hpp"

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
        MappingFieldIs<uint64_t>,
        WorkloadMapConditionIs<Template2Type<ContainerIsNearEmpty>>,
        PushWayBalancerIs<Int2Type<false>>,
        PopWayBalancerIs<Int2Type<false>>
    > DefaultSettings;

template<class... Params>
using QueueTraits =
    EasyTraits<DefaultSettings, 0, Params...>;

template<template<class, class> class PolicyWrapper, class Policy>
using UseQueuePolicy =
    ReplaceDefaultSettingWithPolicy<DefaultSettings, PolicyWrapper, Policy>;

} } }
