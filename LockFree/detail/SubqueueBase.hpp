#pragma once

#include<atomic>

namespace TricksAndThings { namespace LockFree { namespace detail
{

template<class Cfg>
class SubqueueBase
    : public Cfg::SubInfoCalls
{
    std::atomic_flag acquired;

    protected:

    SubqueueBase() : acquired(ATOMIC_FLAG_INIT){}

    public:

    bool acquire(){ return !acquired.test_and_set(); }
};

} } }
