#pragma once

#include<atomic>
#include<mutex>
#include<condition_variable>

namespace TricksAndThings { namespace detail {

template<class TaskQueue>
class Manager
{
    size_t numOfWorkers;

    std::mutex lock;
    std::condition_variable check;
    std::atomic<size_t> numOfWorkersAwaiting;

    TaskQueue &queue;

    public:

    Manager(size_t n, TaskQueue &q)
        : numOfWorkers(0),
          numOfWorkersAwaiting(0),
          queue(q){}

    template<class W> void workerStopped(const W &);

    template<class W> void workerResumed(const W &)
    { -- numOfWorkersAwaiting; }

    size_t onNewWorker()
    { return numOfWorkers ++ ; }

    template<class WorktimeStrategies>
    void wait(size_t threshold);
};

} }
#include "Manager.tcc"
