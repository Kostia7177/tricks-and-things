#pragma once

#include<atomic>
#include<mutex>
#include<condition_variable>

namespace TricksAndThings { namespace detail
{

class ConsumerIdle
{
    std::mutex lock;
    std::condition_variable check;
    std::atomic<bool> awaiting;
    public:
    ConsumerIdle() : awaiting(false){}
    template<class F> void until(F f);
    void kick(){ check.notify_one(); }
    bool isAwaiting(){ return awaiting; }
    void interrupt()
    {
        if (!awaiting) { return; }
        std::unique_lock<std::mutex> locker(lock);
        if (awaiting) { kick(); }
    }
};
} }
#include "ConsumerIdle.tcc"
