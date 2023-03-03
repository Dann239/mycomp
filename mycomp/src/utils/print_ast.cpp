#include "mycomp/ast.hpp"
#include "mycomp/utils/print_ast.hpp"

#include "mycomp/utils/token_to_string.hpp"

#include <fmt/core.h>
#include <memory>

using namespace mycomp;
using enum AstNodeType;

struct AstPrinter: AstVisitor {
    void visit(const AstNodeBody<MODULE>& v) override {
        printPrefix();
        fmt::print("Module\n");
        depth++;
        for(auto& e : v.decls)
            e->acceptVisitor(*this);
        depth--;
    }
    void visit(const AstNodeBody<TYPE>& v) override {
        printPrefix();
        fmt::print("Type\n");
        depth++;
        printToken(v.body);
        depth--;
    }

    void visit(const AstNodeBody<FUNCTION_DECL>& v) override {
        printPrefix();
        fmt::print("FunctionDecl\n");
        depth++;
        printToken(v.name);
        v.type.acceptVisitor(*this);
        depth--;
    }
    void visit(const AstNodeBody<VARIABLE_DECL>& v) override {
        printPrefix();
        fmt::print("VariableDecl\n");
        depth++;
        v.type.acceptVisitor(*this);
        printToken(v.name);
        v.value->acceptVisitor(*this);
        depth--;
    }

    void visit(const AstNodeBody<ASSIGNMENT_STMT>& v) override {
        printPrefix();
        fmt::print("AssignmentStmt\n");
        depth++;
        printToken(v.var);
        v.value->acceptVisitor(*this);
        depth--;
    }
    void visit(const AstNodeBody<EXPR_STMT>& v) override {
        printPrefix();
        fmt::print("ExprStmt\n");
        depth++;
        v.body->acceptVisitor(*this);
        depth--;
    }

    void visit(const AstNodeBody<UNARY_EXPR>& v) override {
        printPrefix();
        fmt::print("UnaryExpr\n");
        depth++;
        printToken(v.op);
        v.expr->acceptVisitor(*this);
        depth--;
    }
    void visit(const AstNodeBody<BINARY_EXPR>& v) override {
        printPrefix();
        fmt::print("BinaryExpr\n");
        depth++;
        v.lhs->acceptVisitor(*this);
        printToken(v.op);
        v.rhs->acceptVisitor(*this);
        depth--;
    }
    void visit(const AstNodeBody<COMPOUND_EXPR>& v) override {
        printPrefix();
        fmt::print("CompoundExpr\n");
        depth++;
        for(auto& e : v.preface)
            std::visit([this](const auto& elem) { elem->acceptVisitor(*this); }, e);
        v.last->acceptVisitor(*this);
        depth--;
    }
    void visit(const AstNodeBody<IF_EXPR>& v) override {
        printPrefix();
        fmt::print("IfExpr\n");
        depth++;
        v.cond->acceptVisitor(*this);
        v.on_true->acceptVisitor(*this);
        v.on_false->acceptVisitor(*this);
        depth--;
    }
    void visit(const AstNodeBody<RETURN_EXPR>& v) override {
        printPrefix();
        fmt::print("ReturnExpr\n");
        depth++;
        v.result->acceptVisitor(*this);
        depth--;
    }
    void visit(const AstNodeBody<LITERAL_EXPR>& v) override {
        printPrefix();
        fmt::print("LiteralExpr\n");
        depth++;
        printToken(v.body);
        depth--;
    }

private:
    int depth = 0;
    void printPrefix() {
        for(int i = 0; i < depth; i++)
            fmt::print("  ");
        fmt::print("\u2514");
    }
    void printToken(const Token& token) {
        printPrefix();
        fmt::print("{}\n", token_to_string(token));
    }
};

std::unique_ptr<AstVisitor> mycomp::make_ast_printer() {
    return std::make_unique<AstPrinter>();
}