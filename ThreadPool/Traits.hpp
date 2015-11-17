#pragma once

#include "../EasyTraits/EasyTraits.hpp"
#include "CfgComponents.hpp"
#include "detail/Statistics.hpp"
#include "../LockFree/Queues/FewToLot2.hpp"
#include "ShutdownStrategies.hpp"
#include "detail/Worker.hpp"
#include "detail/Manager.hpp"

namespace TricksAndThings {
namespace detail {

template<class T>
using DefaultThreadPoolQueue =
    LockFree::Queues::FewToLot2Preconfigured<T>;

}

typedef DefaultSettingsBox
    <
        StatisticsAre<detail::NullStatistics>,
        QueueIs<Template2Type<detail::DefaultThreadPoolQueue>>,
        ShutdownPolicyIs<Int2Type<gracefulShutdown>>,
        CheckArg1Type<Int2Type<true>>,
        WorkerIs<Template2Type<detail::Worker>>,
        ManagerIs<Template2Type<detail::Manager>>
    > ThreadPoolDefaultSettings;

template<class... Params>
using ThreadPoolTraits =
    EasyTraits<ThreadPoolDefaultSettings, 0, Params...>;

template<template<class, class> class PolicyWrapper, class Policy>
using UseThreadPoolPolicy =
    ReplaceDefaultSettingWithPolicy<ThreadPoolDefaultSettings, PolicyWrapper, Policy>;

}
