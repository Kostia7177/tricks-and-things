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

#include "Traits.hpp"
#include "detail/Entry.hpp"
#include "../Tools/ObjHolder.hpp"
#include "../detail/UsefulDefs.hpp"

namespace TricksAndThings { namespace LockFree { namespace Queues
{

template<typename T, class Cfg = Traits<>>
class GeneralPurpose
    : public Cfg::InfoCalls
{
    static const size_t pageSize = Cfg::pageSize;

    enum
    {
        pushWay,
        popWay,
        numOfWays,

        back = pushWay,
        front = popWay
    };

    struct Page
    {
        typedef ObjHolder<Page, typename Cfg::template Storage<Page>> Holder;

        detail::Entry<T> data[pageSize];

        Holder tail[numOfWays];
        SizeAtomic pointers[numOfWays];
        std::atomic<uint64_t> used;

        Page();

        void refOn()    { ++ used; }
        void unRef();
        void clear();
    };

    typedef typename Cfg::template Storage<Page> Storage;

    typedef typename Page::Holder Holder;
    Holder notepad[numOfWays];

    typedef typename Holder::Ptr PagePtr;

    template<size_t> void tearOffDirtyPages();
    template<size_t> void clear();

    public:

    GeneralPurpose(){}
    ~GeneralPurpose();

    void push(T &&);
    bool pop(T &);
};

} } }
#include "GeneralPurpose.tcc"
