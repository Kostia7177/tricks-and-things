#pragma once

namespace TricksAndThings{

template<template<typename...> class Arg>
struct Template2Type
{ template<typename... T> using Template = Arg<T...>; };

}
