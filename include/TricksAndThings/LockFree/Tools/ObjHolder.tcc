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

namespace TricksAndThings { namespace LockFree {

template<class T, class Storage>
ObjHolder<T, Storage>::Ptr::~Ptr()
{
    if (ptr) { ptr->unRef(); }
}

template<class T, class Storage>
typename ObjHolder<T, Storage>::Ptr &ObjHolder<T, Storage>::Ptr::operator=(Ptr &&arg)
{
    swap(std::move(arg));
    return *this;
}

template<class T, class Storage>
void ObjHolder<T, Storage>::makeIfNull(
    Itself &p,
    T *&newPtr)
{
    if (newPtr)
    {
        Storage::utilize(newPtr);
        newPtr = 0;
    }
    if (!p.ptr)
    {
        newPtr = Storage::make();
        p.ptr = newPtr;
    }
}

template<class T, class Storage>
ObjHolder<T, Storage>::~ObjHolder()
{
    if (T *ptr = itself.load(mem::acquire).ptr)
    {
        ptr->unRef();
    }
}

template<class T, class Storage>
typename ObjHolder<T, Storage>::Ptr ObjHolder<T, Storage>::getPtrStrict()
{
    T *newPtr = 0;
    detail::casIf<Free>(itself, [&](Itself &p)
                                {
                                    makeIfNull(p, newPtr);
                                    ++ p.inProgress;
                                    return true;
                                });
    return *this;
}

template<class T, class Storage>
T *ObjHolder<T, Storage>::getSubj()
{
    T *newPtr = 0;
    detail::casIf<Free>(itself, [&](Itself &p)
                                { return makeIfNull(p, newPtr), true; });
    return itself.load(mem::relaxed).ptr;
}

template<class T, class Storage>
typename ObjHolder<T, Storage>::Ptr ObjHolder<T, Storage>::get()
{
    detail::casIf<AsConsumer>(itself, [](Itself &p)
                                      {
                                        if (!p.ptr) { return false; }
                                        ++ p.inProgress;
                                        return true;
                                      });
    return *this;
}

template<class T, class Storage>
void ObjHolder<T, Storage>::setIfNull(T *arg)
{
    detail::casIf<Free>(itself, [=](Itself &p)
                                {
                                    if (p.ptr) { return false; }
                                    p.ptr = arg;
                                    return true;
                                });
}

template<class T, class Storage>
T *ObjHolder<T, Storage>::refere2()
{
    T *subj = itself.load(mem::relaxed).ptr;
    if (subj)
    {
        subj->refOn();
        detail::casIf<AsProducer>(itself, [](Itself &p)
                                          {
                                            -- p.inProgress;
                                            return true;
                                          });
    }
    return subj;
}

template<class T, class Storage>
template<class C>
bool ObjHolder<T, Storage>::swapIf(
    Ptr &&arg,
    C condition)
{
    T *orig;
    if (!detail::casIf<>(itself, [&](Itself &p)
                                 {
                                    if (p.inProgress
                                        || !condition(p.ptr, arg.ptr))
                                    {
                                        return false;
                                    }
                                    orig = p.ptr;
                                    p.ptr = arg.ptr;
                                    return true;
                                 }))
    {
        return false;
    }
    arg.ptr = orig;
    return true;
}

} }
