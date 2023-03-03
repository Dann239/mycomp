#pragma once

namespace mycomp {

namespace detail {

template<template<typename...> typename Template, typename T>
constexpr bool specialization_of = false;

template<template<typename...> typename Template, typename... Ts>
constexpr bool specialization_of<Template, Template<Ts...>> = true;


template<template<auto...> typename Template, typename T>
constexpr bool specialization_of2 = false;

template<template<auto...> typename Template, auto... vals>
constexpr bool specialization_of2<Template, Template<vals...>> = true;

}

template<typename T, template<typename...> typename Template>
concept SpecializationOf = detail::specialization_of<Template, T>;

template<typename T, template<auto...> typename Template>
concept SpecializationOf2 = detail::specialization_of2<Template, T>;

}
