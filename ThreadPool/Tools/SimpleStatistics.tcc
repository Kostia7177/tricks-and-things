#include<cstring>

namespace TricksAndThings
{

inline std::ostream &operator<<(
    std::ostream &s,
    const SimpleStatistics &d)
{
    std::copy(d.data, d.data + SimpleStatistics::numOf,
              std::ostream_iterator<size_t>(s, "\t"));
    s << (double)d.sumSize / d.data[SimpleStatistics::calls]
      << std::endl;
    return s;
}

template<class Queue>
void SimpleStatistics::store(Queue &q)
{
    ++ data[calls];
    data[lastSize] = q->subSize();
    sumSize += data[lastSize];
}

void SimpleStatistics::clear()
{
    memset(data, 0, sizeof(data));
    sumSize = 0;
}

}
