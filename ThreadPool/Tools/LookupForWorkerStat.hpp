#pragma once

#include<cstddef>
#include "../../LockFree/Queues/Tools/ConsumerConditionIfc.hpp"

namespace TricksAndThings
{

template<template<class> class Condition, class PreCondition>
class LookupForWorkerStat
{
    ConsumerConditionIfc *condition;
    PreCondition preCondition;

    public:

    LookupForWorkerStat()
        : condition(0),
          checkRange(this){}

    template<typename... Args>
    LookupForWorkerStat(
        ConsumerConditionIfc *p,
        Args &&... args)
        : condition(p),
          preCondition(std::forward<Args>(args)...),
          checkRange(this){}

    LookupForWorkerStat &operator=(const LookupForWorkerStat &arg)
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
        LookupForWorkerStat *holder;

        public:

        RangeChecker(LookupForWorkerStat *p)
            : holder(p){}

        bool operator()(size_t idx) const
        { return holder->condition->checkRange(idx); }
    } checkRange;

    template<class W>
    using WorkerStat = Condition<W>;
};

}
