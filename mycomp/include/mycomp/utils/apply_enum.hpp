#pragma once

#include <magic_enum.hpp>

#include <type_traits>

namespace mycomp {

template<typename E, typename F>
requires std::is_enum_v<E>
constexpr decltype(auto) apply_enum(F&& f) {
    return [&]<auto... inds>(std::index_sequence<inds...>) -> decltype(auto) {
        return f(std::integer_sequence<E, magic_enum::enum_value<E>(inds)...>{});
    }(std::make_index_sequence<magic_enum::enum_count<E>()>{});
}

template<typename E, template<E...> typename F>
requires std::is_enum_v<E>
using ApplyEnum = std::remove_pointer_t<decltype(
    apply_enum<E>([]<E... inds>(std::integer_sequence<E, inds...>) -> F<inds...>* { return {}; })
)>;

}
