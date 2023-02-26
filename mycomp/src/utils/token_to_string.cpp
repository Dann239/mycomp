#include "mycomp/utils/token_to_string.hpp"

#include <magic_enum.hpp>
#include <fmt/core.h>

namespace mycomp {

std::string token_to_string(const Token& token) {
    auto to_str = []<typename T>(const T& sth) -> std::string {
        if constexpr (std::is_same_v<T, std::monostate>)
            return "";
        else
            return fmt::format("{}", sth);
    };
    return fmt::format(
        "{}({}) at [{}, {})",
        magic_enum::enum_name(token.tokenType),
        std::visit(to_str, token.payload),
        token.begin_pos,
        token.end_pos
    );
}

}
