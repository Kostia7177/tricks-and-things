/*
    Use free. Be free. Be lock-free!!

    Copyright (C) 2014, 2015  Konstantin U. Zozoulia

    candid.71 -at- mail -dot- ru

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include<functional>

namespace TricksAndThings
{

template<class... Cfg>
template<class F, typename... Args>
void ThreadPool<Cfg...>::applyOnWorkers(
    F f,
    Args &&...args)
{
    for (WorkerPtr &worker : workers)
    {// worker->f(args...); - that's what here is in fact;
        (worker.get()->*f)(std::forward<Args>(args)...);
    }
}

template<class... Cfg>
template<class P, class F, typename... Args>
void ThreadPool<Cfg...>::scheduleSw(
    P &&p,
    F f, 
    Args &&...args)
{
    static thread_local typename TaskQueue::ProviderSideProxy queuePtr = &tasks;

    auto wrappedF = std::bind(f, std::forward<Args>(args)...);

    typedef Task<decltype(wrappedF), P> Task2Push;
    queuePtr->push(std::make_shared<Task2Push>(wrappedF,
                                               std::forward<P>(p)));

    queuePtr->apply([&](size_t idx) { workers[idx]->newDataAppeared(); });
}

template<class... Cfg>
ThreadPool<Cfg...>::ThreadPool(size_t n)
    : tasks(n),
      manager(n, tasks),
      workerCondition(workers)
{
    tasks.setWorkloadMapCondition(&workerCondition);
    for (size_t idx = 0; idx < n; ++ idx)
    {
        workers.emplace_back(new Worker(tasks, manager));
    }
}

template<class... Cfg>
ThreadPool<Cfg...>::~ThreadPool()
{
    applyOnWorkers(&Worker::completeWork);
    applyOnWorkers(&Worker::join);
}

template<class... Cfg>
template<class F, typename... Args>
std::future<typename std::result_of<F(Args...)>::type> ThreadPool<Cfg...>::submit(
    F f,
    Args &&... args)
{
    typedef typename std::result_of<F(Args...)>::type Ret;
    detail::WithPromise<Ret> promiseWrapper;
    std::future<Ret> ret(promiseWrapper.getFuture());

    scheduleSw(std::move(promiseWrapper), f, std::forward<Args>(args)...);

    return ret;
}

}
