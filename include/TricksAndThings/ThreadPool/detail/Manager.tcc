
namespace TricksAndThings { namespace detail {

template<class TaskQueue>
template<class Worker>
void Manager<TaskQueue>::workerStopped(const Worker &worker)
{
    ++ numOfWorkersAwaiting;
    check.notify_one();
}

template<class TaskQueue>
template<class WorktimeStrategies>
void Manager<TaskQueue>::wait(size_t threshold)
{
    std::unique_lock<std::mutex> locker(lock);
    while (WorktimeStrategies::continueAwaiting(numOfWorkersAwaiting.load(),
                                                numOfWorkers,
                                                queue.size(),
                                                threshold))
    {
        check.wait(locker);
    }
}

} }
