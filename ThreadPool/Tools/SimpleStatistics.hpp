#pragma once

#include<iterator>

namespace TricksAndThings
{

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

}
#include "SimpleStatistics.tcc"
