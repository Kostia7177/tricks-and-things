#pragma once

#include "../../EasyTraits/EasyTraits.hpp"
#include "../../LockFree/Queues/FewToLot2.hpp"
#include "../detail/Worker.hpp"
#include "../detail/Manager.hpp"
#include "CfgComponents.hpp"
#include "SimpleStatistics.hpp"
#include "ShutdownStrategies.hpp"
#include "LookupForWorkerStat.hpp"
#include "WorkerNotBusy.hpp"

namespace TricksAndThings
{

namespace Lfq = LockFree::Queues;

typedef DefaultSettingsBox
    <
        StatisticsAre<NullType>,
        QueueIs<Template2Type
                <
                    Lfq::FewToLot2,
                    Lfq::UseQueuePolicy<Lfq::WithInfoCalls, Int2Type<true>>,
                    Lfq::UseQueuePolicy<Lfq::ExitedConsumersMapIs, NullType>,
                    Lfq::UseQueuePolicy<Lfq::WorkloadMapConditionIs,
                                        LookupForWorkerStat<WorkerNotBusy,
                                                            LockFree::ContainerIsNearEmpty<0>>>
                >>,
        ShutdownPolicyIs<Int2Type<gracefulShutdown>>,
        WorkerIs<Template2Type<detail::Worker>>,
        ManagerIs<Template2Type<detail::Manager>>
    > ThreadPoolDefaultSettings;

struct ThreadPoolDomain {};

template<class... Params>
using ThreadPoolTraits =
    EasyTraits<ThreadPoolDefaultSettings, ThreadPoolDomain, Params...>;

template<template<class, class> class PolicyWrapper, class Policy>
using UseThreadPoolPolicy =
    ReplaceDefaultSettingWithPolicy<ThreadPoolDefaultSettings, PolicyWrapper, Policy>;

}
