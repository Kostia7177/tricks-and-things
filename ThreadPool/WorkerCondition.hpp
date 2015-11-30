#pragma once

#include<cstddef>
#include "../LockFree/Queues/ConsumerConditionIfc.hpp"

namespace TricksAndThings
{

template<class PreCondition>
class LookupForWorkerCondition
{
    ConsumerConditionIfc *condition;
    PreCondition preCondition;

    public:

    LookupForWorkerCondition() : condition(0) {}

    template<typename... Args>
    LookupForWorkerCondition(
        ConsumerConditionIfc *p,
        Args &&... args)
        : condition(p),
          preCondition(std::forward<Args>(args)...){}

    template<typename... Args>
    bool operator()(size_t idx, Args &&... args) const
    { return !preCondition(idx, std::forward<Args>(args)...) ?
                false
                : condition->check(idx); }
};

template<class Team>
class WorkerNotBusy
    : public ConsumerConditionIfc
{
    Team &workers;

    public:

    WorkerNotBusy(Team &t)
        : workers(t){}

    bool check(size_t idx)
    { return !workers[idx]->busy(); }
};

}
