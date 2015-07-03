#pragma once

#include<thread>

namespace TricksAndThings { namespace detail{

template<class TaskQueue, class ShutdownPolicy, class Statistics>
class Worker
{
    typename TaskQueue::ConsumerIdle idle;
    size_t idx;
    bool workCompleted;
    Statistics statistics;

    std::thread thread;

    public:
    typedef ShutdownPolicy WorktimeStrategies;

    template<class Manager> Worker(TaskQueue &, Manager &);
    ~Worker();

    size_t getIdx() const   { return idx; }
    void newDataAppeared()  { idle.interrupt(); }
    void completeWork();
    void clearStatistics()  { statistics.clear(); }
    
    void showStatistics(std::ostream &s)
    { s << statistics; }
};

} }
#include "Worker.tcc"
