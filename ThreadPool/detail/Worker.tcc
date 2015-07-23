
namespace TricksAndThings { namespace detail {

template<class TaskQueue, ShutdownStrategies shutdownPolicy, class Statistics>
template<class Manager>
Worker<TaskQueue, shutdownPolicy, Statistics>::Worker(
    TaskQueue &tasks,
    Manager &manager)
    : idle(tasks),
      idx(manager.onNewWorker()),
      workCompleted(false),
      thread([&]
             {
                typename TaskQueue::ConsumerSideProxy queuePtr = &tasks;
                typename TaskQueue::Type taskPtr;
                do
                {
                    if (WorktimeStrategies
                        ::taskIsAppliable(!workCompleted)
                        && taskPtr.get())
                    {
                        statistics.store(queuePtr);
                        taskPtr->doIt();
                    }
                    taskPtr.reset();
                    queuePtr->notifyBalancer(*this);
                    if (!queuePtr->pop(taskPtr))
                    {
                        manager.workerStopped(*this);
                        statistics.stopped();

                        idle.until([&]
                                   { return queuePtr->pop(taskPtr)
                                            || workCompleted; });

                        manager.workerResumed(*this);
                    }
                }
                while (WorktimeStrategies
                       ::goOn(taskPtr.get(), !workCompleted));
             })
{
}

template<class TaskQueue, ShutdownStrategies shutdownPolicy, class Statistics>
Worker<TaskQueue, shutdownPolicy, Statistics>::~Worker()
{
    if (thread.joinable()) { thread.join(); }
}

template<class TaskQueue, ShutdownStrategies shutdownPolicy, class Statistics>
void Worker<TaskQueue, shutdownPolicy, Statistics>::completeWork()
{
    workCompleted = true;
    idle.kick();
}

} }
