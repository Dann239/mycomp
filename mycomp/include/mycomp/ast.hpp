#pragma once

#include "lex.hpp"

#include <concepts>
#include <memory>
#include <variant>

namespace mycomp {

struct AstVisitor;

struct AstNode {
    virtual void acceptVisitor(AstVisitor& visitor) const = 0;
    virtual ~AstNode() = default;
};

struct Declaration : virtual AstNode {
};
struct Statement : virtual AstNode {
};
struct Expression : virtual AstNode {
};

template<typename T>
struct AstVisitable : virtual AstNode {
    void acceptVisitor(AstVisitor& visitor) const final;
};

struct Module :
    AstVisitable<Module>,
    virtual AstNode
{
    std::vector<std::unique_ptr<Declaration>> decls;
};

struct Type :
    AstVisitable<Type>,
    virtual AstNode
{
    Token body;
};


// Declarations

struct FunctionDecl :
    AstVisitable<FunctionDecl>,
    Declaration
{
    Token name;
    Type type;
    // ??? std::vector<std::pair<Token, Type>> args;
    // ??? TODO
};

struct VariableDecl :
    AstVisitable<VariableDecl>,
    Declaration
{
    Token name;
    Type type;
    std::unique_ptr<Expression> value;
};


// Statements

struct AssignmentStmt :
    AstVisitable<AssignmentStmt>,
    Declaration
{
    Token var;
    std::unique_ptr<Expression> value;
};

struct ExprStmt :
    AstVisitable<ExprStmt>,
    Declaration
{
    std::unique_ptr<Expression> body;
};


// Expressions

struct UnaryExpr :
    AstVisitable<UnaryExpr>,
    Expression
{
    Token op;
    std::unique_ptr<Expression> expr;
};

struct BinaryExpr :
    AstVisitable<BinaryExpr>,
    Expression
{
    Token op;
    std::unique_ptr<Expression> lhs, rhs;
};

struct CompoundExpr :
    AstVisitable<CompoundExpr>,
    Expression
{
    std::vector<
        std::variant<
            std::unique_ptr<Declaration>,
            std::unique_ptr<Statement>
        >
    > preface;

    std::unique_ptr<Expression> last;
};

struct IfExpr :
    AstVisitable<IfExpr>,
    Expression
{
    std::unique_ptr<Expression> cond, on_true, on_false;
};

struct ReturnExpr :
    AstVisitable<ReturnExpr>,
    Expression
{
    std::unique_ptr<Expression> result;
};

struct LiteralExpr :
    AstVisitable<LiteralExpr>,
    Expression
{
    Token body;
};


struct AstVisitor {
    virtual void visit(const Module& v) = 0;
    virtual void visit(const Type& v) = 0;

    virtual void visit(const FunctionDecl& v) = 0;
    virtual void visit(const VariableDecl& v) = 0;

    virtual void visit(const AssignmentStmt& v) = 0;
    virtual void visit(const ExprStmt& v) = 0;

    virtual void visit(const UnaryExpr& v) = 0;
    virtual void visit(const BinaryExpr& v) = 0;
    virtual void visit(const CompoundExpr& v) = 0;
    virtual void visit(const IfExpr& v) = 0;
    virtual void visit(const ReturnExpr& v) = 0;
    virtual void visit(const LiteralExpr& v) = 0;

    virtual ~AstVisitor() = default;
};

template<typename T>
void AstVisitable<T>::acceptVisitor(AstVisitor& visitor) const {
    visitor.visit(dynamic_cast<const T&>(*this));
}

}
