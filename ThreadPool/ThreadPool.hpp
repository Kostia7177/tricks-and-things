#pragma once
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

#include "detail/RecurserIfc.hpp"
#include "Traits.hpp"
#include<vector>
#include<memory>

namespace TricksAndThings
{

template<class... Params>
class ThreadPool
{
    typedef ThreadPoolTraits<Params...> Cfg;
    public:
    typedef detail::RecurserPtr RecurserPtr;
    private:
    typedef detail::NoRecurser NoRecurser;
    typedef detail::WithRecurser WithRecurser;
    struct TaskIfc
    {
        virtual ~TaskIfc(){}
        virtual void doIt(RecurserPtr &&) = 0;
    };
    public:
    typedef typename Cfg::template Queue<std::shared_ptr<TaskIfc>> TaskQueue;
    private:

    TaskQueue tasks;

    template<class F>
    class Task
        : public TaskIfc
    {
        F payload;

        public:

        Task(F &&f) : payload(std::forward<F>(f)){}

        virtual void doIt(RecurserPtr &&recurser)
        { payload(std::move(recurser)); }
    };

    typename Cfg::template Manager<TaskQueue> manager;

    typedef typename Cfg::template Worker<TaskQueue, Int2Type<Cfg::shutdownPolicy>, typename Cfg::Statistics> Worker;
    typedef std::unique_ptr<Worker> WorkerPtr;
    std::vector<WorkerPtr> workers;

    typename Cfg::template WorkerCondition<decltype(workers)> workerCondition;

    template<class F, class... Args>
    void applyOnWorkers(F, Args &&...);

    typedef typename TaskQueue::ProviderSideProxy ProviderSideProxy;
    template<class F, typename... Args>
    void pushIntoQueue(ProviderSideProxy &, F &&, Args &&...);

    template<class F, typename... Args>
    void scheduleCore(F &&, RecurserPtr &&, Args &&...);
    template<class F, typename... Args>
    void scheduleSw(NoRecurser, ProviderSideProxy &, RecurserPtr &&,F &&, Args &&...);
    template<class F, typename... Args>
    void scheduleSw(WithRecurser, ProviderSideProxy &, RecurserPtr &&, F &&, Args &&...);

    public:

    ThreadPool(size_t);
    ~ThreadPool()       { applyOnWorkers(&Worker::completeWork); }

    template<class F, class... Args>
    void schedule(F, Args &&...);
    template<class F, typename... Args>
    void schedule(F, RecurserPtr &&, Args &&...);

    size_t size() const { return workers.size(); }

    void wait(size_t threshold = 0)
    { manager.wait(threshold); }

    void clearStatistics()
    { applyOnWorkers(&Worker::clearStatistics); }

    void showStatistics(std::ostream &s)
    { applyOnWorkers(&Worker::showStatistics, s); }
};
}
#include "detail/ThreadPool.tcc"
