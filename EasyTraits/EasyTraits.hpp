#pragma once
/*
    Trait it easy.

    Copyrignt (C) 2015 Konstantin U. Zozoulia

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

// A simple toolkit to create policy-based classes.
//
// Warning: using this technique, you may have to
//          equip your computer with bike pedals -
//          to make scrolling compiler's error
//          messages faster and more healthy.
// On other side,
//          everything is fine.
//
// For the impatients - usage see at
//
//      ../LockFree/Queues/Traits.hpp
//      ../ThreadPool/Traits.hpp
//      https://github.com/Kostia7177/proto-transformer/blob/master/include/ProtoTransformer/Proto.hpp
//                                                                                      ..../examples/<all>/Proto.hpp
//                                                                                                          |Server.cpp
//                                                                                                          |Client.cpp
// The main idea have been taken from
// 'C++ Templates - The Complete Guide' book
// by David Vandevoorde and Nicolai M. Josuttis (http://www.josuttis.com/tmplbook/)
// Part III, chapter 16.1;
//
#include "detail.hpp"
#include "../ParamPackManip/Params2TypesHierarchy.hpp"

namespace TricksAndThings
{

// An empty box for all traits, policies, strategies, constants etc that
// are to be passed into a class that is to be configured. Fill this box
// with traits, that are to be default for a class that is to be tuned.
template<class... Policy>
using DefaultSettingsBox = Params2TypesHierarchy<Policy...>;    // <------------+
// Parameter values must be wrapped by                                          |
// template structures by the following                                         |
// pattern:                                                                     |
//                                                                              |
//  template<class P, class Base = NullType> // from ../Tools/NullType.hpp      |
//  struct AnyParameterIs : virtual Base                                        |
//  { typedef P AnyParameter; };                                                |
//                                                                              |
// Numeric (integer) constants and templates must be enclosed to Int2Type       |
// and Template2Type additional wrappers.                                       |
// (from ../Tools/Int2Type.hpp and ../Tools/Template2Type.hpp)                  |
//                                                                              |
// At your code it will be looking like                                         |
//                                                                              |
//    typedef DefaultSettingsBox                                                |
//              <                                                               |
//                  ParameterOneIs<FirstParam>,                                 |
//                  SecondParamIs<AnotherParam>,                                |
//                  // somewhere defined enum { valueOfParam = 42 };            |
//                  ThirdNumericParameterIs<Int2Type<valueOfParam>>,            |
//                  TheFouthOneIs<Template2Type<SomeTemplateNotAType>>          |
//                  // ...etc                                                   |
//              > BoxFullOfSettings;                                            |
//                                                                              |
// Configuration itself - this is a thing, that will be a template              |
// parameter of your tunable class.                                             |
template<class Settings,    // -- a box with your default settings -------------+
         class D,           // -- domain feature, usable if you want to
                            //    pass something specific into your traits
                            //    box (such a trait of this traits group),
                            //    or if you want to 'group up' your parameters
                            //    to any sub-sets, marked by this feature;
                            //    in this case must be unique;
                            //    in the very most cases will be a simple
                            //    struct SomeDomain {};
                            //    or even my precious detail::NullType,
                            //    if you really shure that you will never
                            //    use it;
         class... WrappedParams>    // -- an order-independent sequence of the
                                    //    parameters, wrapped by the 'policy replacers' ----+
                                    //    that will replace the parameter values from       |
                                    //    a default policy set;                             |
struct EasyTraits   //                                                                      |
    : detail::DefaultCfg<Settings>,     //                                                  |
      detail::BaseNumerator<D,  //                                                          |
                            typename detail             //                                  |
                                     ::CreateIdxSequence<sizeof...(WrappedParams)>  //      |
                                     ::Type,                //                              |
                            WrappedParams...>   //                                          |
{                                               //                                          |
    typedef D Domain;   //                                                                  |
};  //                                                                                      |
//                                                                                          |
// Replacer for a policy that is to be used instead of default one.                         |
template<class Settings, template<class, class> class PolicyWrapper, class Policy>  // <----+
struct ReplaceDefaultSettingWithPolicy : public PolicyWrapper<Policy, typename Settings::Type> {};
//
// It will be a nice idea to define an alias for
// your class-oriented policy-replacer, such as:
//
//   template<template<class, class> class Wrapper, class Policy>
//   using UseYourOwnClassPolicy =
//      ReplaceDefaultSettingWithPolicy<BoxFullOfSettings, Wrapper, Policy>;
//
// ...and for traits:
//
//   template<class... Params>
//   using YourOwnClassTraits =
//      EasyTraits<BoxFullOfSettings, 0, Params...>;
//
// Now, to configure your policy-based class, say:
//
//   YourPolicyBasedClass
//      <YourOwnClassTraits
//          <
//              UseYourOwnClassPolicy<SecondParamIs, SomeCustomValue>,  // rendom and
//              UseYourOwnClassPolicy<TheFourthOneIs, Template2Type<AnyTemplate>>   // order-independent
//              // etc                                                              // parameters set
//          >
//      > instance;
//
// - examples see at ../LockFree/Queues/Traits.hpp
//                   ../ThreadPool/Traits.hpp
//                   https://github.com/Kostia7177/proto-transformer/blob/master/include/ProtoTransformer/Proto.hpp
//                                                                                                  ...../examples/<all>/Proto.hpp
//                                                                                                                       |Server.cpp
//                                                                                                                       |Client.cpp
//
// ...I do think it really looks pretty well. Do ya?..

}
