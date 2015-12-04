#pragma once

#include<cstddef>
#include "../LockFree/Queues/Tools/ConsumerConditionIfc.hpp"

namespace TricksAndThings
{

template<class PreCondition>
class LookupForWorkerCondition
{
    ConsumerConditionIfc *condition;
    PreCondition preCondition;

    public:

    LookupForWorkerCondition()
        : condition(0),
          checkRange(this){}

    template<typename... Args>
    LookupForWorkerCondition(
        ConsumerConditionIfc *p,
        Args &&... args)
        : condition(p),
          preCondition(std::forward<Args>(args)...),
          checkRange(this){}

    LookupForWorkerCondition &operator=(const LookupForWorkerCondition &arg)
    {
        condition = arg.condition;
        preCondition = arg.preCondition;
        return *this;
    }

    template<typename... Args>
    bool operator()(size_t idx, Args &&... args) const
    { return !preCondition(idx, std::forward<Args>(args)...) ?
                false
                : condition->check(idx); }

    class RangeChecker
    {
        LookupForWorkerCondition *holder;

        public:

        RangeChecker(LookupForWorkerCondition *p)
            : holder(p){}

        bool operator()(size_t idx) const
        { return holder->condition->checkRange(idx); }
    } checkRange;
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

    bool checkRange(size_t idx)
    { return idx < workers.size(); }
};

}
