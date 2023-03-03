#pragma once

#include "ast.hpp"

#include "utils/apply_enum.hpp"

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

using AstVisitorImplAll = ApplyEnum<AstNodeType, AstVisitorImpl>;

}

struct AstVisitor :
    detail::AstVisitorImplAll
{};

template<AstNodeType type>
void AstNodeConcrete<type>::acceptVisitor(AstVisitor& visitor) const {
    detail::AstVisitorImpl<type>& visitor_impl = visitor;
    visitor_impl.visit(body_);
}

}
