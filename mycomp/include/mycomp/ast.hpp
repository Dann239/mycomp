#pragma once

#include "lex.hpp"

#include <memory>
#include <type_traits>
#include <utility>

namespace mycomp {

enum class AstNodeType {
    MODULE,
    TYPE,

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

template<AstNodeType type> struct ConcreteAstNode;

struct AstVisitor;

struct AstNode {
    virtual void acceptVisitor(AstVisitor& visitor) const = 0;
    virtual ~AstNode() = default;
};

template<AstNodeType>
struct AstNodeBody;

template<AstNodeType type>
struct ConcreteAstNode :
    AstNodeBody<type>::Parent
{
    void acceptVisitor(AstVisitor& visitor) const final; // defined in ast_visitor.hpp

    ConcreteAstNode(AstNodeBody<type> body) :
        body_(std::move(body))
    {}

    AstNodeBody<type> body_;
};

template<AstNodeType type>
auto make_ast_node(AstNodeBody<type> body) {
    return std::unique_ptr<typename AstNodeBody<type>::Parent>(new ConcreteAstNode<type>{std::move(body)});
}

struct Declaration : AstNode {
};
struct Statement : AstNode {
};
struct Expression : AstNode {
};

template<> struct AstNodeBody<AstNodeType::MODULE> {
    using Parent = AstNode;
    std::vector<std::unique_ptr<Declaration>> decls;
};

template<> struct AstNodeBody<AstNodeType::TYPE> {
    using Parent = AstNode;
    Token body;
};


// Declarations

template<> struct AstNodeBody<AstNodeType::FUNCTION_DECL> {
    using Parent = Declaration;
    Token name;
    ConcreteAstNode<AstNodeType::TYPE> type;
    // ??? std::vector<std::pair<Token, Type>> args;
    // ??? TODO
};

template<> struct AstNodeBody<AstNodeType::VARIABLE_DECL> {
    using Parent = Declaration;
    Token name;
    ConcreteAstNode<AstNodeType::TYPE> type;
    std::unique_ptr<Expression> value;
};


// Statements

template<> struct AstNodeBody<AstNodeType::ASSIGNMENT_STMT> {
    using Parent = Statement;
    Token var;
    std::unique_ptr<Expression> value;
};

template<> struct AstNodeBody<AstNodeType::EXPR_STMT> {
    using Parent = Statement;
    std::unique_ptr<Expression> body;
};


// Expressions

template<> struct AstNodeBody<AstNodeType::UNARY_EXPR> {
    using Parent = Expression;
    Token op;
    std::unique_ptr<Expression> expr;
};

template<> struct AstNodeBody<AstNodeType::BINARY_EXPR> {
    using Parent = Expression;
    Token op;
    std::unique_ptr<Expression> lhs, rhs;
};

template<> struct AstNodeBody<AstNodeType::COMPOUND_EXPR> {
    using Parent = Expression;
    std::vector<
        std::variant<
            std::unique_ptr<Declaration>,
            std::unique_ptr<Statement>
        >
    > preface;

    std::unique_ptr<Expression> last;
};

template<> struct AstNodeBody<AstNodeType::IF_EXPR> {
    using Parent = Expression;
    std::unique_ptr<Expression> cond, on_true, on_false;
};

template<> struct AstNodeBody<AstNodeType::RETURN_EXPR> {
    using Parent = Expression;
    std::unique_ptr<Expression> result;
};

template<> struct AstNodeBody<AstNodeType::LITERAL_EXPR> {
    using Parent = Expression;
    Token body;
};

}
