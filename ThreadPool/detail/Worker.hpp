#pragma once

#include<thread>
#include<boost/mpl/map.hpp>
#include<boost/mpl/at.hpp>

namespace TricksAndThings {

enum ShutdownStrategies
{
    gracefulShutdown,
    shutdownImmediate
};

namespace detail
{

namespace Bm = boost::mpl;

template<class TaskQueue, ShutdownStrategies shutdownPolicy, class Statistics>
class Worker
{
    typename TaskQueue::ConsumerIdle idle;
    std::atomic<bool> threadStarted;
    bool workCompleted;
    Statistics statistics;

    std::thread thread;

    struct GracefulShutdown
    {
        static bool goOn(bool taskNotEmpty, bool notExiting)
        { return taskNotEmpty || notExiting; }
        static bool taskIsAppliable(bool)
        { return true; }
    };
    struct ShutdownImmediate
    {
        static bool goOn(bool, bool notExiting)
        { return notExiting; }
        static bool taskIsAppliable(bool notExiting)
        { return notExiting; }
    };

    typedef typename Bm::map
        <
            Bm::pair<Bm::int_<gracefulShutdown>, GracefulShutdown>,
            Bm::pair<Bm::int_<shutdownImmediate>, ShutdownImmediate>
        >::type AllWorktimeStrategies;

    typedef typename Bm::at<AllWorktimeStrategies, Bm::int_<shutdownPolicy>>::type WorktimeStrategies;

    public:

    template<class Manager> Worker(TaskQueue &, Manager &);
    ~Worker();

    void newDataAppeared()  { idle.interrupt(); }
    void completeWork();
    void clearStatistics()  { statistics.clear(); }
    
    void showStatistics(std::ostream &s)
    { s << statistics; }
};

} }
#include "Worker.tcc"
