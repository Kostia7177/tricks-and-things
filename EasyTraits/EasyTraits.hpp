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

#include "detail.hpp"

namespace TricksAndThings
{

template<class Settings,
         int idx,
         class T1 = detail::DefaultCfg<Settings>,
         class T2 = detail::DefaultCfg<Settings>,
         class T3 = detail::DefaultCfg<Settings>,
         class T4 = detail::DefaultCfg<Settings>,
         class T5 = detail::DefaultCfg<Settings>,
         class T6 = detail::DefaultCfg<Settings>,
         class T7 = detail::DefaultCfg<Settings>,
         class T8 = detail::DefaultCfg<Settings>>
struct EasyTraits
    : detail::BaseUniquizer<T1, 1, idx>,
      detail::BaseUniquizer<T2, 2, idx>,
      detail::BaseUniquizer<T3, 3, idx>,
      detail::BaseUniquizer<T4, 4, idx>,
      detail::BaseUniquizer<T5, 5, idx>,
      detail::BaseUniquizer<T6, 6, idx>,
      detail::BaseUniquizer<T7, 7, idx>,
      detail::BaseUniquizer<T8, 8, idx>
{
    enum { selectorIdx = idx };
};

template<class Settings, template<class, class> class PolicyWrapper, class Policy>
struct ReplaceDefaultSettingWithPolicy : public PolicyWrapper<Policy, typename Settings::Type> {};

template<class Settings, template<template<typename> class, class> class PolicyWrapper, template<typename> class Policy>
struct ReplaceDefaultSettingWithPolicyTemplate : public PolicyWrapper<Policy, typename Settings::Type> {};

}
