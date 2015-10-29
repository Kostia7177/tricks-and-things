#pragma once

namespace TricksAndThings { namespace detail {

template<class Settings>
struct DefaultCfg : virtual public Settings::Type {};

template<class Base, int, int>
struct BaseUniquizer : public Base {};

} }
