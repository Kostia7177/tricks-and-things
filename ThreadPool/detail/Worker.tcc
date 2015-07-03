
namespace TricksAndThings { namespace detail {

template<class TaskQueue, class ShutdownPolicy, class Statistics>
template<class Manager>
Worker<TaskQueue, ShutdownPolicy, Statistics>::Worker(
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
                        ::taskIsAppliable(taskPtr.get(),
                                          !workCompleted))
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
                while (WorktimeStrategies::goOn(taskPtr.get(),
                                                !workCompleted));
             })
{
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
