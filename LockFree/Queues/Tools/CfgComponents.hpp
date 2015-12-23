#pragma once

#include "../../../Tools/NullType.hpp"
#include "../../../Tools/Int2Type.hpp"
#include<cstddef>

namespace TricksAndThings { namespace LockFree { namespace Queues
{

template<class S, class Base = NullType>
struct StorageIs : virtual Base
{ template<typename T>
  using Storage = typename S::template Template<T>; };

template<class C, class Base = NullType> struct WithInfoCalls;

template<class Base>
struct WithInfoCalls<Int2Type<true>, Base> : virtual Base
{
    class InfoCalls
    {
        SizeAtomic value;
        protected:
        void incrSize()     { ++ value; }
        void decrSize()     { -- value; }
        InfoCalls() : value(0) {}
        ~InfoCalls() {}
        public:
        bool empty() const  { return !size(); }
        size_t size() const { return value.load(); }
    };
};

template<class Base>
struct WithInfoCalls<Int2Type<false>, Base> : virtual Base
{
    class InfoCalls
    {
        template<class T>
        static size_t fail(T)
        {
            struct Inaccessable{};
            static_assert(std::is_same<T, Inaccessable>::value,
                          "\n\n\tThe 'size()' and 'empty()' methos are not implemented by default."
                          "\n\tTo call them use the 'WithInfoCalls' configuration trait;\n");
            return 0;
        }
        protected:
        void incrSize()     {}
        void decrSize()     {}
        public:
        bool empty() const  { return !size(); }
        size_t size() const { return fail(NullType()); }
    };
};

template<class C, class Base = NullType>
struct WithSubInfoCalls : virtual Base
{ typedef typename WithInfoCalls<C, Base>::InfoCalls SubInfoCalls; };

template<class S, class Base = NullType>
struct PageSizeIs : virtual Base
{ static const size_t pageSize = S::value; };

template<class N, class Base = NullType>
struct NumOfConsumersLimitIs : virtual Base
{ static const size_t numOfConsumersLimit = N::value; };

template<class M, class Base = NullType>
struct WorkloadMapIs : virtual Base
{ template<typename Field, class Condition>
  using WorkloadMap =
      typename M::template Template<Field, Condition>; };

template<class F, class Base = NullType>
struct MappingFieldIs : virtual Base
{ typedef F MappingField; };

template<class C, class Base = NullType>
struct WorkloadMapConditionIs : virtual Base
{ typedef C WorkloadMapCondition; };

template<class N, class Base = NullType>
struct WithPushWayBalancer : virtual Base
{ enum { pushWayBalancer = N::value }; };

template<class N, class Base = NullType>
struct WithPopWayBalancer : virtual Base
{ enum { popWayBalancer = N::value }; };

template<class C, class Base = NullType>
struct ConsumerIdleIs : virtual Base
{ typedef C ConsumerIdle; };

template<class M, class Base = NullType>
struct ExitedConsumersMapIs : virtual Base
{ template<class Field>
  using ExitedConsumersMap = typename M::template Template<Field>; };

template<class Base>
struct ExitedConsumersMapIs<NullType, Base> : virtual Base
{
    template<typename>
    struct ExitedConsumersMap
    {
        void inject(size_t)                 {}
        bool eject(size_t *)                { return false; }
        template<typename C>
        bool getLowest(size_t *, const C &) { return false; }
        bool contains(size_t)               { return false; }
    };
};

} } }
