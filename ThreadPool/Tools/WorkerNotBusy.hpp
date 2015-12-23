#pragma once

#include<cstddef>

namespace TricksAndThings
{

template<class Team>
class WorkerNotBusy
    : public ConsumerConditionIfc
{
    Team &workers;

    public:

    WorkerNotBusy(Team &t)
        : workers(t){}

    bool check(size_t idx)
    { return !workers[idx]->busy(); }

    bool checkRange(size_t idx)
    { return idx < workers.size(); }
};

}
