#pragma once

namespace TricksAndThings { namespace LockFree { namespace detail
{
template<class Subj>
class SubqueueIsEmpty
{
    Subj *ptr;
    public:
    SubqueueIsEmpty(Subj *s) : ptr(s){}
    template<class... Params>
    bool operator()(Params &&...)
    { return ptr->subSize() == 0; }
};
} } }
