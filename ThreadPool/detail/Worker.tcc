#include "IsRecurserInside.hpp"

namespace TricksAndThings { namespace detail
{

template<class TaskQueue, class ShutdownPolicy, class Statistics>
template<class Manager>
Worker<TaskQueue, ShutdownPolicy, Statistics>::Worker(
    TaskQueue &tasks,
    Manager &manager)
    : threadStarted(false),
      workCompleted(false),
      thread([&]
             {
                manager.onNewWorker();
                typename TaskQueue::ConsumerSideProxy queuePtr = &tasks;
                threadStarted = true;
                typename TaskQueue::Type taskPtr;
                do
                {
                    if (WorktimeStrategies
                        ::taskIsAppliable(!workCompleted)
                        && taskPtr.get())
                    {
                        statistics.store(queuePtr);
                        runTaskSw(IsRecurserInside<TaskQueue>(),
                                  taskPtr,
                                  queuePtr);
                    }
                    taskPtr.reset();
                    if (!queuePtr->pop(taskPtr))
                    {
                        manager.workerStopped(*this);
                        statistics.stopped();

                        idle.until([&]
                                   {
                                    manager.workerResumed(*this);
                                    if (queuePtr->pop(taskPtr)
                                        || workCompleted)
                                    { return true; }

                                    manager.workerStopped(*this);

                                    return false;
                                   });
                    }
                }
                while (WorktimeStrategies
                       ::goOn(taskPtr.get(), !workCompleted));
             })
{
    while (!threadStarted);
}

template<class TaskQueue, class ShutdownPolicy, class Statistics>
Worker<TaskQueue, ShutdownPolicy, Statistics>::~Worker()
{
    if (thread.joinable()) { thread.join(); }
}

template<class TaskQueue, class ShutdownPolicy, class Statistics>
void Worker<TaskQueue, ShutdownPolicy, Statistics>::completeWork()
{
    workCompleted = true;
    idle.kick();
}

} }
