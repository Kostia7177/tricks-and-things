#pragma once

#include<thread>
#include "../ShutdownStrategies.hpp"

namespace TricksAndThings { namespace detail
{

template<class TaskQueue, class ShutdownPolicy, class Statistics>
class Worker
{
    typename TaskQueue::ConsumerIdle idle;
    std::atomic<bool> threadStarted;
    bool workCompleted;
    Statistics statistics;

    std::thread thread;

    template<int, int = 0> struct AllWorktimeStrategies;

    template<int unused>
    struct AllWorktimeStrategies<gracefulShutdown, unused>
    {
        static bool goOn(bool taskNotEmpty, bool notExiting)
        { return taskNotEmpty || notExiting; }
        static bool taskIsAppliable(bool)
        { return true; }
    };

    template<int unused>
    struct AllWorktimeStrategies<shutdownImmediate, unused>
    {
        static bool goOn(bool, bool notExiting)
        { return notExiting; }
        static bool taskIsAppliable(bool notExiting)
        { return notExiting; }
    };

    typedef AllWorktimeStrategies<ShutdownPolicy::value> WorktimeStrategies;

    public:

    template<class Manager> Worker(TaskQueue &, Manager &);
    ~Worker();

    void newDataAppeared()  { idle.interrupt(); }
    bool busy()             { return !idle.isAwaiting(); }
    void completeWork();
    void clearStatistics()  { statistics.clear(); }
    
    void showStatistics(std::ostream &s)
    { s << statistics; }
};

} }
#include "Worker.tcc"
