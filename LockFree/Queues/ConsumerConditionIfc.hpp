#pragma once

#include<cstddef>

namespace TricksAndThings
{

struct ConsumerConditionIfc
{
    virtual bool check(size_t) = 0;
    virtual ~ConsumerConditionIfc() {}
};

}
