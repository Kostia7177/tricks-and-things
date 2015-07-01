#pragma once
/*
    Use free. Be free. Be lock-free!!

    Copyright (C) 2014, 2015  Konstantin U. Zozoulia

    candid.71 -at- mail -dot- ru

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "../detail/SubqueueBase.hpp"
#include "../Tools/ObjHolder.hpp"
#include "../detail/Entry.hpp"
#include "../detail/UsefulDefs.hpp"
#include "Traits.hpp"
#include "WithParallelConsumers.hpp"

namespace TricksAndThings { namespace LockFree
{
namespace detail
{

template<typename T, class C>
class FewToSingle
    : public SubqueueBase<C>
{
    public:
    typedef T Type;
    typedef C Cfg;
    private:

    static const size_t pageSize = Cfg::pageSize;

    struct Page
    {
        typedef typename Cfg::template Storage<Page> Storage;
        typedef ObjHolder<Page, Storage> Holder;

        Entry<Type> data[pageSize];

        size_t front;       // -- index of an element that will be
                            // extracted by the nearest 'pop' call;
                            // there's just 1 thread that calls 'pop' -
                            // so, no cuncurrency expected;
        SizeAtomic back;    // -- index of the first free cell (that will
                            // be used by the nearest 'push' call);
                            // 'push' way is shared by a lot of threads -
                            // so, must be atomic;
        Holder tail;        // -- next page;
        std::atomic<int> used;  // -- reference counter;

        public:

        Page() : front(0), back(0),
                           used(2)  // initially refered by 'Holder'
                                    // (either 'last' or 'tail') on
                                    // 'push' way and 'first' pointer
                                    // on 'pop' way - so, used by 2
                                    // referers;
        {}

        void refOn() { ++ used; }
        void unRef();
    };

    typedef typename Page::Holder Holder;
    typedef typename Holder::Ptr PagePtr;

    Page *first;
    Holder last;

    void tearOffDirtyPages();

    public:

    FewToSingle() : first(0) {}
    ~FewToSingle();

    void init(SizeAtomic *);
    bool ready() { return first; }
    void push(Type &&);
    bool pop(Type &);

    class ClientHub
    {
        SizeAtomic consumerIdx;

        public:

        ClientHub() : consumerIdx(0){}

        class Ptr
        {   // proxy - immitates a pointer to a 'ClientHub'
            // to implement 'getConsumerIdx' and 'syncIdx';
            // supports concurrent multi-thread operating
            // with notepad index when 'push' chooses the
            // notepad for writing a data;
            ClientHub * const ptr;  // target hub itself;
            size_t valueBak;    // thread-spec. value backup;

            public:

            Ptr(ClientHub *p) : ptr(p), valueBak(0) {}

            Ptr *operator->() { return this; }

            size_t getConsumerIdx();
            bool syncIdx(size_t *);
        };

        void onNewProvider(){}
        void onProviderExited(){}
    };
};

} // <-- namespace detail

namespace Queues
{
template<typename T, class Cfg = Traits<>> using FewToLot = WithParallelConsumers<detail::FewToSingle<T, Cfg>>;
}

} }
#include "FewToLot.tcc"
