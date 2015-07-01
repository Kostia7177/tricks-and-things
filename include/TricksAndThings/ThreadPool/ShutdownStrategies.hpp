#pragma once

#include<cstddef>

namespace TricksAndThings {

struct GracefulShutdown
{
    static bool goOn(bool taskNotEmpty, bool notExiting)
    { return taskNotEmpty || notExiting; }

    static bool taskIsAppliable(bool taskNotEmpty, bool)
    { return taskNotEmpty; }

    static bool continueAwaiting(
        size_t stoppedWorkers,
        size_t workers,
        size_t tasksPending,
        size_t threshold)
    { return (workers - stoppedWorkers + tasksPending) > threshold; }
};

}
