#pragma once

#include<cstddef>

namespace TricksAndThings { namespace detail
{

template<class Settings>
struct DefaultCfg : virtual Settings::Type {};

template<class Base, int, class>
struct BaseUniquizer : Base {};

// the index sequencer - really cool thing,
// idea taken from
// http://ldionne.com/2015/11/29/efficient-parameter-pack-indexing/
// and
// http://talesofcpp.fusionfenix.com/post-22/true-story-efficient-packing
// for the more detail comments you better see there;
//
// metaporgram: building the index sequence;
template<size_t... idxs>    // <-- this parameter-pack is a thing, for what all
                            // the following code (down to [*]) is written
struct IdxSequence {};      // <-- just a holder for it's template parameter;

// while (n)
// {
template<size_t n, size_t... idxs>
struct IdxSequenceGenerator
{
    typedef typename IdxSequenceGenerator<n - 1, n, idxs...>::Type Type;    // idxs += n -- ;
};
// }

template<size_t... idxs>
struct IdxSequenceGenerator<0, idxs...>
{   // border case (n == 0) - the parameter-pack
    // is ready - store it to IdxSequence<>;
    typedef IdxSequence<idxs...> Type;
};

template<size_t n>
struct CreateIdxSequence
{
    typedef typename IdxSequenceGenerator<n>::Type Type;
};  // [*] end of the metaprogram;

template<class Domain, class Idxs, class... Params>
struct BaseNumerator;

template<class Domain, size_t... idxs, class... Params>
struct BaseNumerator<Domain, IdxSequence<idxs...>, Params...>
    : BaseUniquizer<Params, idxs, Domain>...
{
};

} }
