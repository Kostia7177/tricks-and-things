#pragma once

#include "../Tools/DefaultAllocatingStorage.hpp"
#include "../Tools/InfoCalls.hpp"
#include "../../Tools/NullType.hpp"

namespace TricksAndThings
{
namespace LockFree
{
namespace Queues
{

template<template<typename> class S = DefaultAllocatingStorage,
         template<class> class C = Components::NoInfoCalls,
         template<class> class SubC = C,
         size_t s = 4096,
         size_t n = 128>
struct Traits
{
    template<typename T> using Storage = S<T>;
    typedef C<NullType> InfoCalls;
    typedef SubC<NullType> SubInfoCalls;
    static const size_t pageSize = s;
    static const size_t numOfConsumersLimit = n;
};

}
}
}
