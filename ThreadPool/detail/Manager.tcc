
namespace TricksAndThings { namespace detail
{

template<class TaskQueue>
template<class Worker>
void Manager<TaskQueue>::workerStopped(const Worker &worker)
{
    std::unique_lock<std::mutex> locker(lock);
    ++ awaitings;
    check.notify_one();
}

template<class TaskQueue>
void Manager<TaskQueue>::wait(size_t threshold)
{
    std::unique_lock<std::mutex> locker(lock);
    while ((numOfWorkers - awaitings.load() + queue.size()) > threshold)
    {
        check.wait(locker);
    }
}

} }
