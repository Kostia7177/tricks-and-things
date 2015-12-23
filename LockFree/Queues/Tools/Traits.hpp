#pragma once

#include "../../../EasyTraits/EasyTraits.hpp"
#include "../../Tools/DefaultAllocatingStorage.hpp"
#include "CfgComponents.hpp"
#include "../../../Tools/Int2Type.hpp"
#include "../../../Tools/Template2Type.hpp"
#include "../../../Tools/NullType.hpp"
#include "../../Tools/BinaryMapper.hpp"
#include "../detail/ConsumerIdle.hpp"

namespace TricksAndThings { namespace LockFree { namespace Queues
{

typedef DefaultSettingsBox
    <
        StorageIs<Template2Type<DefaultAllocatingStorage>>,
        WithInfoCalls<Int2Type<false>>,
        WithSubInfoCalls<Int2Type<false>>,
        PageSizeIs<Int2Type<4096>>,
        NumOfConsumersLimitIs<Int2Type<128>>,
        WorkloadMapIs<Template2Type<BinaryMapperCond>>,
        ExitedConsumersMapIs<Template2Type<BinaryMapper>>,
        MappingFieldIs<uint64_t>,
        WorkloadMapConditionIs<ContainerIsNearEmpty<>>,
        WithPushWayBalancer<Int2Type<false>>,
        WithPopWayBalancer<Int2Type<false>>,
        ConsumerIdleIs<TricksAndThings::detail::ConsumerIdle>
    > DefaultSettings;

struct QueuesDomain {};

template<class... Params>
using QueueTraits =
    EasyTraits<DefaultSettings, QueuesDomain, Params...>;

template<template<class, class> class PolicyWrapper, class Policy>
using UseQueuePolicy =
    ReplaceDefaultSettingWithPolicy<DefaultSettings, PolicyWrapper, Policy>;

} } }
