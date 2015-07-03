
namespace TricksAndThings { namespace detail {

template<class TaskQueue>
template<class Worker>
void Manager<TaskQueue>::workerStopped(const Worker &worker)
{
    ++ numOfWorkersAwaiting;
    check.notify_one();
}

template<class TaskQueue>
void Manager<TaskQueue>::wait(size_t threshold)
{
    std::unique_lock<std::mutex> locker(lock);
    while ((numOfWorkers - numOfWorkersAwaiting.load() + queue.size()) > threshold)
    {
        check.wait(locker);
    }
}

} }
