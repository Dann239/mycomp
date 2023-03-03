#pragma once

#include "lex.hpp"
#include "utils/specialization_of.hpp"

#include <memory>
#include <type_traits>
#include <utility>

namespace mycomp {

enum class AstNodeType {
    MODULE,
    PRIMITIVE_TYPE,

    FUNCTION_DECL,
    VARIABLE_DECL,

    ASSIGNMENT_STMT,
    EXPR_STMT,

    UNARY_EXPR,
    BINARY_EXPR,
    COMPOUND_EXPR,
    IF_EXPR,
    RETURN_EXPR,
    LITERAL_EXPR
};

template<AstNodeType type> struct AstNodeConcrete;

struct AstVisitor;

struct AstNode {
    virtual void acceptVisitor(AstVisitor& visitor) const = 0;
    virtual ~AstNode() = default;
};

template<AstNodeType>
struct AstNodeBody;


enum class AstCategoryType {
    DECLARATION,
    STATEMENT,
    EXPRESSION,
    TYPE,
    MODULE
};

template<AstCategoryType type>
struct AstCategory : AstNode {
};

template<AstCategoryType type>
using AstPtr = std::unique_ptr<AstCategory<type>>;

using DeclPtr = AstPtr<AstCategoryType::DECLARATION>;
using StmtPtr = AstPtr<AstCategoryType::STATEMENT>;
using ExprPtr = AstPtr<AstCategoryType::EXPRESSION>;
using TypePtr = AstPtr<AstCategoryType::TYPE>;
using ModulePtr = AstPtr<AstCategoryType::MODULE>;

template<typename T, AstCategoryType type>
concept AstBodyOf = SpecializationOf2<T, AstNodeBody> && T::category == type;

template<AstNodeType type>
struct AstNodeConcrete :
    AstCategory<AstNodeBody<type>::category>
{
    void acceptVisitor(AstVisitor& visitor) const final; // defined in ast_visitor.hpp

    AstNodeConcrete(AstNodeBody<type> body) :
        body_(std::move(body))
    {}

    AstNodeBody<type> body_;
};

template<AstNodeType type>
auto make_ast_node(AstNodeBody<type> body) {
    return AstPtr<AstNodeBody<type>::category>(new AstNodeConcrete<type>{std::move(body)});
}


// MODULE

template<> struct AstNodeBody<AstNodeType::MODULE> {
    static constexpr auto category = AstCategoryType::MODULE;
    std::vector<DeclPtr> decls;
};


// TYPES

template<> struct AstNodeBody<AstNodeType::PRIMITIVE_TYPE> {
    static constexpr auto category = AstCategoryType::TYPE;
    Token body;
};


// Declarations

template<> struct AstNodeBody<AstNodeType::FUNCTION_DECL> {
    static constexpr auto category = AstCategoryType::DECLARATION;
    Token name;
    TypePtr type;
    // ??? std::vector<std::pair<Token, Type>> args;
    // ??? TODO
};

template<> struct AstNodeBody<AstNodeType::VARIABLE_DECL> {
    static constexpr auto category = AstCategoryType::DECLARATION;
    Token name;
    TypePtr type;
    ExprPtr value;
};


// Statements

template<> struct AstNodeBody<AstNodeType::ASSIGNMENT_STMT> {
    static constexpr auto category = AstCategoryType::STATEMENT;
    Token var;
    ExprPtr value;
};

template<> struct AstNodeBody<AstNodeType::EXPR_STMT> {
    static constexpr auto category = AstCategoryType::STATEMENT;
    ExprPtr body;
};


// Expressions

template<> struct AstNodeBody<AstNodeType::UNARY_EXPR> {
    static constexpr auto category = AstCategoryType::EXPRESSION;
    Token op;
    ExprPtr expr;
};

template<> struct AstNodeBody<AstNodeType::BINARY_EXPR> {
    static constexpr auto category = AstCategoryType::EXPRESSION;
    Token op;
    ExprPtr lhs, rhs;
};

template<> struct AstNodeBody<AstNodeType::COMPOUND_EXPR> {
    static constexpr auto category = AstCategoryType::EXPRESSION;
    std::vector<
        std::variant<
            DeclPtr,
            StmtPtr
        >
    > preface;

    ExprPtr last;
};

template<> struct AstNodeBody<AstNodeType::IF_EXPR> {
    static constexpr auto category = AstCategoryType::EXPRESSION;
    ExprPtr cond, on_true, on_false;
};

template<> struct AstNodeBody<AstNodeType::RETURN_EXPR> {
    static constexpr auto category = AstCategoryType::EXPRESSION;
    ExprPtr result;
};

template<> struct AstNodeBody<AstNodeType::LITERAL_EXPR> {
    static constexpr auto category = AstCategoryType::EXPRESSION;
    Token body;
};

}
