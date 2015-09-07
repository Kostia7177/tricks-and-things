#pragma once

#include "UsefulDefs.hpp"

namespace TricksAndThings { namespace LockFree { namespace detail
{

class ThreadSafeClientHub
{
    SizeAtomic consumerIdx;

    public:

    ThreadSafeClientHub() : consumerIdx(0){}

    class Ptr
    {   // proxy - immitates a pointer to a 'ClientHub'
        // to implement 'getConsumerIdx' and 'syncIdx';
        // supports concurrent multi-thread operating
        // with notepad index when 'push' chooses the
        // notepad for writing a data;
        ThreadSafeClientHub * const ptr;  // target hub itself;
        size_t valueBak;    // thread-spec. value backup;

        public:

        Ptr(ThreadSafeClientHub *p) : ptr(p), valueBak(0) {}

        Ptr *operator->() { return this; }

        size_t getConsumerIdx()
        {
            valueBak = ptr->consumerIdx.load();
            return valueBak;
        }

        bool syncIdx(size_t *idx)
        {
            if (ptr->consumerIdx.compare_exchange_strong(valueBak,
                        *idx))
            {
                return true;
            }
            *idx = valueBak;

            return false;
        }
    };

    void onNewProvider(){}
    void onProviderExited(){}
};
} } }
