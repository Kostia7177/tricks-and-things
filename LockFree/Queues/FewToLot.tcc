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

#include "../detail/casIf.hpp"

namespace TricksAndThings { namespace LockFree { namespace detail
{

template<typename T, class C>
void FewToSingle<T, C>::Page::unRef()
{
    if (! -- used)
    {
        Storage::utilize(this);
    }
}

template<typename T, class C>
void FewToSingle<T, C>::tearOffDirtyPages()
{   // note, that here is tearing off the dirty pages
    // happens only at the 'push' way!
    if (last.template getRaw<mem::relaxed>()->back.load() < pageSize)
    {   // simple 'if(last.back < pageSize)' actially;
        // in other words - page has some free space
        // (and hence is not dirty);
        // in most cases page is not dirty indeed, so
        // instead (and before) building the PagePtr
        // (and turning up and down it's reference
        // counter) we do a fast check here;
        return;
    }

    PagePtr page;
    do
    {
        page = last.get();
    }
    while (page->back.load(mem::relaxed) == pageSize
           && last.swapIf(page->tail, [&](Page *pg1, Page *pg2)
                                      { return page == pg1 && pg2; }));
}

template<typename T, class C>
FewToSingle<T, C>::~FewToSingle()
{
    for (Page *page = first;
         page;
         page = page->tail.template getRaw<mem::relaxed>())
    {
        page->back.store(pageSize);
        page->unRef();
    }

    if (!last.template isNull<mem::relaxed>())
    {
        while (!last.get()->tail.template isNull<mem::relaxed>())
        {
            tearOffDirtyPages();
        }
    }
}

template<typename T, class C>
void FewToSingle<T, C>::init(SizeAtomic *numOfConsumers)
{
    first = last.getSubj();
    ++ *numOfConsumers;
}

template<typename T, class C>
void FewToSingle<T, C>::push(Type &&p)
{
    PagePtr keeper = last.get();

    Page *page = keeper.get();
    size_t idx;
    while (!casIf<>(page->back, [&](size_t &value)
                                {
                                    return value < pageSize ?
                                            idx = value ++ , true
                                            : false;
                                }))
    {
        page = page->tail.getSubj();
    }

    tearOffDirtyPages();

    auto entry = &page->data[idx];
    entry->itself = std::move(p);
    entry->ready.store(true, mem::release);
    this->incrSize();
}

template<typename T, class C>
bool FewToSingle<T, C>::pop(Type &ret)
{
    if (first->front == pageSize)
    {
        Page *newFirst = first->tail.template getRaw<mem::acquire>();
        if (!newFirst){ return false; }
        Page *passed = first;
        first = newFirst;
        passed->unRef();
    }

    auto &entry = first->data[first->front];
    if (!entry.ready.load(mem::acquire))
    {
        return false;
    }
    ret = std::move(entry.itself);
    ++ first->front;
    this->decrSize();

    return true;
}

} } }
