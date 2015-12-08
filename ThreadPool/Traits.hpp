#pragma once

#include "CfgComponents.hpp"
#include "../EasyTraits/EasyTraits.hpp"
#include "detail/Statistics.hpp"
#include "../LockFree/Queues/FewToLot2.hpp"
#include "ShutdownStrategies.hpp"
#include "detail/Worker.hpp"
#include "detail/Manager.hpp"
#include "WorkerCondition.hpp"

namespace TricksAndThings
{
namespace detail
{

namespace Lfq = LockFree::Queues;

template<typename T>
using DefaultThreadPoolQueue =
    Lfq::FewToLot2Preconfigured<T, Lfq::FewToLot2Traits
                                    <
                                        Lfq::UseQueuePolicy<Lfq::InfoCallsAre, Template2Type<Lfq::WithInfoCalls>>,
                                        Lfq::UseQueuePolicy<Lfq::WorkloadMapConditionIs,
                                                            LookupForWorkerCondition<LockFree::ContainerIsNearEmpty<0>>>
                                    >>;
} // <-- namespace detail

typedef DefaultSettingsBox
    <
        StatisticsAre<detail::NullStatistics>,
        QueueIs<Template2Type<detail::DefaultThreadPoolQueue>>,
        ShutdownPolicyIs<Int2Type<gracefulShutdown>>,
        CheckArg1Type<Int2Type<true>>,
        WorkerIs<Template2Type<detail::Worker>>,
        ManagerIs<Template2Type<detail::Manager>>,
        WorkerConditionIs<Template2Type<WorkerNotBusy>>
    > ThreadPoolDefaultSettings;

struct ThreadPoolDomain {};
template<class... Params>
using ThreadPoolTraits =
    EasyTraits<ThreadPoolDefaultSettings, ThreadPoolDomain, Params...>;

template<template<class, class> class PolicyWrapper, class Policy>
using UseThreadPoolPolicy =
    ReplaceDefaultSettingWithPolicy<ThreadPoolDefaultSettings, PolicyWrapper, Policy>;

}
