#pragma once

#include "ast.hpp"

#include <magic_enum.hpp>

namespace mycomp {

namespace detail {

template<AstNodeType... types>
struct AstVisitorImpl :
    AstVisitorImpl<types>...
{};

template<AstNodeType type>
struct AstVisitorImpl<type> {
    virtual void visit(const AstNodeBody<type>& v) = 0;
    virtual ~AstVisitorImpl() = default;
};

using AstVisitorImplAll = std::remove_pointer_t<decltype(
    []<auto... inds>
    (std::index_sequence<inds...>)
    -> AstVisitorImpl<magic_enum::enum_value<AstNodeType>(inds)...>*
    { return {}; }
    (std::make_index_sequence<magic_enum::enum_count<AstNodeType>()>{})
)>;

}

struct AstVisitor :
    detail::AstVisitorImplAll
{};

template<AstNodeType type>
void ConcreteAstNode<type>::acceptVisitor(AstVisitor& visitor) const {
    detail::AstVisitorImpl<type>& visitor_impl = visitor;
    visitor_impl.visit(body_);
}

}
