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
#include "IsRecurserInside.hpp"
#include "../../SignatureManip/GetSignatureFeature.hpp"
#include "../../Tools/ParamFilter.hpp"

namespace TricksAndThings
{

template<class F, bool desired, bool switchOnOff> struct Arg1IsRecurser;

template<class F, bool unusedHere>
struct Arg1IsRecurser<F, unusedHere, true>
{
    typedef typename std::remove_reference<F>::type ClearedF;
    typedef typename GetSignatureFeature<ClearedF, 1>::Param FirstParam;
    enum
    {
        value = std::is_same<FirstParam, detail::RecurserPtr>::value
    };
};

template<class F, bool desired>
struct Arg1IsRecurser<F, desired, false>
{
    enum { value = desired };
};

template<class... Cfg>
template<class F, class... Args>
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
template<class F, typename... Args>
void ThreadPool<Cfg...>::pushIntoQueue(
    ProviderSideProxy &queuePtr,
    F &&f,
    Args &&... args)
{
    typedef ParamFilter<detail::IsRecurserInside<TaskQueue>,
                        F,
                        RecurserPtr,
                        Args...> WrappedF;

    typedef Task<WrappedF> Task2Push;

    queuePtr->push(std::make_shared<Task2Push>(WrappedF(std::forward<F>(f),
                                                        std::forward<Args>(args)...)));

    queuePtr->apply([&](size_t idx){ workers[idx]->newDataAppeared(); });
}

template<class... Cfg>
template<class F, typename... Args>
void ThreadPool<Cfg...>::scheduleCore(
    F &&f,
    RecurserPtr &&recurser,
    Args &&... args)
{
    static thread_local ProviderSideProxy queuePtr = &tasks;

    scheduleSw(detail::IsRecurserInside<TaskQueue>(),
               queuePtr,
               std::forward<RecurserPtr>(recurser),
               std::forward<F>(f),
               std::forward<Args>(args)...);
}

template<class... Cfg>
template<class F, typename... Args>
void ThreadPool<Cfg...>::scheduleSw(
    NoRecurser,
    ProviderSideProxy &queuePtr,
    RecurserPtr &&,
    F &&f,
    Args &&... args)
{
    static_assert(!Arg1IsRecurser<F, false, Cfg::checkArg1Type>::value,
                  "\n\n\tRecurserPtr specified as a first argument for a working thread, while Queue "
                  "\n\tof ThreadPool has not Recurser inside as a nested class - either drop RecurserPtr "
                  "\n\tfrom worker's signature, or reconfigure ThreadPoll with other, recursive, Queue.\n");
    pushIntoQueue(queuePtr, std::forward<F>(f), std::forward<Args>(args)...);
}

template<class... Cfg>
template<class F, typename... Args>
void ThreadPool<Cfg...>::scheduleSw(
    WithRecurser,
    ProviderSideProxy &queuePtr,
    RecurserPtr &&recurser,
    F &&f,
    Args &&... args)
{
    static_assert(Arg1IsRecurser<F, true, Cfg::checkArg1Type>::value,
                  "\n\n\tA nested class Recurser detected inside a Queue object of a ThreadPool, so either"
                  "\n\tthe RecurserPtr must be the first parameter of working thread, or reconfigure the"
                  "\n\tThreadPool with non-recursive Queue.\n");
    if (recurser.get() && recurser->overridePush())
    {
        f(std::forward<RecurserPtr>(recurser), std::forward<Args>(args)...);
    }
    else
    {
        pushIntoQueue(queuePtr, std::forward<F>(f), std::forward<Args>(args)...);
    }
}

template<class... Cfg>
ThreadPool<Cfg...>::ThreadPool(size_t n)
    : tasks(n),
      manager(n, tasks),
      workerCondition(workers)
{
    for (size_t idx = 0; idx < n; ++ idx)
    {
        workers.emplace_back(new Worker(tasks, manager));
    }
    tasks.setWorkloadMapCondition(&workerCondition);
}

template<class... Cfg>
template<class F, class... Args>
void ThreadPool<Cfg...>::schedule(
    F f,
    Args &&... args)
{
    scheduleCore(f,
                 RecurserPtr(nullptr),
                 std::forward<Args>(args)...);
}

template<class... Cfg>
template<class F, class... Args>
void ThreadPool<Cfg...>::schedule(
    F f,
    RecurserPtr &&recurser,
    Args &&... args)
{
    scheduleCore(f,
                 std::forward<RecurserPtr>(recurser),
                 std::forward<Args>(args)...);
}

}
