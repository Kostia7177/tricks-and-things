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

#include "../detail/UsefulDefs.hpp"
#include<utility>

namespace TricksAndThings { namespace LockFree
{

template<class T, class Storage>
class ObjHolder
{
    struct Itself
    {
        T *ptr;
        uint32_t inProgress;
        Itself() noexcept : ptr(0), inProgress(0){}
    };

    alignas(dwordSize) std::atomic<Itself> itself;
    T *ptr;

    void makeIfNull(Itself &, T *&);

    public:

    class Ptr
    {
        friend class ObjHolder;

        T *ptr;

        void swap(Ptr &&arg) { std::swap(ptr, arg.ptr); }

        public:

        Ptr() : ptr(0){}
        Ptr(const Ptr &) = delete;

        template<class H>
        Ptr(H &h) : ptr(h.refere2()){}

        Ptr(Ptr &&arg) : ptr(0)
        { swap(std::move(arg)); }

        ~Ptr();

        Ptr &operator=(Ptr &&);
        Ptr &operator=(const Ptr &)         = delete;
        void *operator new(size_t) throw()  = delete;
        T *operator->() const               { return ptr; }
        bool operator==(const T *p) const   { return p == ptr; }

        bool isNull() const                 { return !ptr; }
        T *get() const                      { return ptr; }
    };

    ObjHolder() : itself(Itself()) {}
    ~ObjHolder();

    Ptr getPtrStrict();
    T *getSubj();
    Ptr get();
    void setIfNull(T *);
    T *refere2();

    template<class C>
    bool swapIf(Ptr &&, C);

    template<mem::Order order>
    bool isNull()   { return !itself.load(order).ptr; }

    template<mem::Order order>
    T *getRaw()     { return itself.load(order).ptr; }
};

} }
#include"ObjHolder.tcc"
