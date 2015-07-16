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
#include <algorithm>

namespace TricksAndThings { namespace LockFree { namespace Queues
{

template<typename T, class Cfg>
GeneralPurpose<T, Cfg>::Page::Page()
    : used(numOfWays)
{
    std::for_each(pointers, pointers + numOfWays, [](SizeAtomic &p)
                                                  { p.store(0); });
}

template<typename T, class Cfg>
void GeneralPurpose<T, Cfg>::Page::unRef()
{
    if (! -- used)
    {
        Storage::utilize(this);
    }
}

template<typename T, class Cfg>
void GeneralPurpose<T, Cfg>::Page::clear()
{
    std::for_each(pointers, pointers + numOfWays, [](SizeAtomic &p)
                                                  { p.store(pageSize); });
}

template<typename T, class Cfg>
template<size_t idx>
void GeneralPurpose<T, Cfg>::tearOffDirtyPages()
{
    Page *firstPage = notepad[idx].template getRaw<mem::relaxed>();

    if (!firstPage
        || firstPage->pointers[idx].load(mem::relaxed) < pageSize)
    {
        return;
    }

    PagePtr page;
    do
    {
        page = notepad[idx].get();
    }
    while (page->pointers[idx].load(mem::relaxed) >= pageSize
           && notepad[idx].swapIf(page->tail[idx], [&](Page *p1, Page *p2)
                                                   { return page == p1 && p2; }));
}

template<typename T, class Cfg>
template<size_t idx>
void GeneralPurpose<T, Cfg>::clear()
{
    Page *page = notepad[idx].template getRaw<mem::relaxed>();
    if (!page) { return; }

    for (;
         page;
         page = page->tail[idx].template getRaw<mem::relaxed>())
    {
        page->clear();
    }

    while (!notepad[idx].get()->tail[idx].template isNull<mem::relaxed>())
    {
        tearOffDirtyPages<idx>();
    }

    PagePtr emptyOne;
    auto anyway = [](Page *, Page *){ return 1; };
    notepad[idx].swapIf(std::move(emptyOne), anyway);
}

template<typename T, class Cfg>
GeneralPurpose<T, Cfg>::~GeneralPurpose()
{
    clear<pushWay>();
    clear<popWay>();
}

template<typename T, class Cfg>
void GeneralPurpose<T, Cfg>::push(T &&p)
{
    size_t idx;
    PagePtr keeper = notepad[pushWay].getPtrStrict();
    Page *page = keeper.get();
    notepad[popWay].setIfNull(page);

    while (!detail::casIf<AsProducer>(page->pointers[back], [&](size_t &value)
                                                            {
                                                                return value < pageSize ?
                                                                    idx = value ++ , true
                                                                    : false;
                                                            }))
    {
        Holder *holder = &page->tail[popWay];
        page = page->tail[pushWay].getSubj();
        holder->setIfNull(page);
    }

    tearOffDirtyPages<pushWay>();

    auto entry = &page->data[idx];
    entry->itself = std::move(p);
    entry->ready.store(true, mem::relaxed);

    this->incrSize();
}

template<typename T, class Cfg>
bool GeneralPurpose<T, Cfg>::pop(T &ret)
{
    PagePtr keeper = notepad[popWay].get();

    Page *page = keeper.get();
    detail::Entry<T> *entry = 0;
    do
    {
        if (!page){ return false; }

        size_t idx = page->pointers[front].load(mem::acquire);
        if (idx < pageSize)
        {
            if (!page->data[idx].ready.load(mem::relaxed))
            {
                return false;
            }

            if (page->pointers[front].compare_exchange_strong(idx,
                                                              idx + 1,
                                                              mem::release,
                                                              mem::relaxed))
            {
                entry = &page->data[idx];
                break;
            }
        }
        else
        {
            page = page->tail[popWay].template getRaw<mem::acquire>();
        }
    }
    while (1);

    this->decrSize();

    ret = std::move(entry->itself);

    tearOffDirtyPages<popWay>();

    return true;
}

} } }
