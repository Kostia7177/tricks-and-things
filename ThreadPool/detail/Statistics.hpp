#pragma once

#include<iterator>

namespace TricksAndThings { namespace detail {

struct NullStatistics
{
    template<class Queue> void store(Queue &){}
    void clear(){}
    void stopped(){}
};

inline static std::ostream &operator<<(std::ostream &s, const NullStatistics &)
{ return s; }

class SimpleStatistics
{
    friend std::ostream &operator<<(std::ostream &, const SimpleStatistics &);

    enum { calls, stops, lastSize, numOf };

    size_t data[numOf];
    size_t sumSize;

    public:

    SimpleStatistics()  { clear(); }

    template<class Queue>
    void store(Queue &);

    inline void clear();
    void stopped()      { ++ data[stops]; }
};

} }
#include "Statistics.tcc"
