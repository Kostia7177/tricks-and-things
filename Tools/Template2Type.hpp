#pragma once

namespace TricksAndThings
{

template<template<typename...> class T, typename... OtherParams>
struct Template2Type
{ template<typename... CoreParams> using Template = T<CoreParams..., OtherParams...>; };

}
