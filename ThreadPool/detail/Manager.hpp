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
    std::atomic<size_t> awaitings;

    TaskQueue &queue;

    public:

    Manager(size_t n, TaskQueue &q)
        : numOfWorkers(0),
          awaitings(0),
          queue(q){}

    template<class W> void workerStopped(const W &);

    template<class W> void workerResumed(const W &)
    { -- awaitings; }

    void onNewWorker() { numOfWorkers ++ ; }

    void wait(size_t threshold);
};

} }
#include "Manager.tcc"
