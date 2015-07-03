#pragma once

#include <type_traits>

template<int arg> using Int2Type = std::integral_constant<int, arg>;
