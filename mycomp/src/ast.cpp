#include "mycomp/ast.hpp"
#include "mycomp/utils/apply_enum.hpp"

#include <concepts>
#include <magic_enum.hpp>
#include <type_traits>
#include <utility>

namespace mycomp {

template<AstNodeType type>
concept TypeValid =
    requires{ typename AstNodeBody<type>; } &&
    requires{ typename AstNodeConcrete<type>; } &&
    requires { {AstNodeBody<type>::category} -> std::common_reference_with<AstCategoryType>; };


static constexpr auto validate_body = []<AstNodeType type>(std::integral_constant<AstNodeType, type>) {
    static_assert(TypeValid<type>);
};

static_assert((magic_enum::enum_for_each<AstNodeType>(validate_body), true));

}