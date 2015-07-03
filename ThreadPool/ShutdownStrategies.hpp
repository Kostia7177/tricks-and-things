#pragma once

#include<cstddef>

namespace TricksAndThings {

struct GracefulShutdown
{
    static bool goOn(bool taskNotEmpty, bool notExiting)
    { return taskNotEmpty || notExiting; }

    static bool taskIsAppliable(bool taskNotEmpty, bool)
    { return taskNotEmpty; }
};

}
