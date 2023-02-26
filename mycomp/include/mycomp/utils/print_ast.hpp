#pragma once

#include "../ast.hpp"
#include "token_to_string.hpp"

#include <fmt/core.h>

namespace mycomp {

struct AstPrinter: AstVisitor {
    void visit(const Module& v) override {
        printPrefix();
        fmt::print("Module\n");
        depth++;
        for(auto& e : v.decls)
            e->acceptVisitor(*this);
        depth--;
    }
    void visit(const Type& v) override {
        printPrefix();
        fmt::print("Type\n");
        depth++;
        printToken(v.body);
        depth--;
    }

    void visit(const FunctionDecl& v) override {
        printPrefix();
        fmt::print("FunctionDecl\n");
        depth++;
        printToken(v.name);
        v.type.acceptVisitor(*this);
        depth--;
    }
    void visit(const VariableDecl& v) override {
        printPrefix();
        fmt::print("VariableDecl\n");
        depth++;
        v.type.acceptVisitor(*this);
        printToken(v.name);
        v.value->acceptVisitor(*this);
        depth--;
    }

    void visit(const AssignmentStmt& v) override {
        printPrefix();
        fmt::print("AssignmentStmt\n");
        depth++;
        printToken(v.var);
        v.value->acceptVisitor(*this);
        depth--;
    }
    void visit(const ExprStmt& v) override {
        printPrefix();
        fmt::print("ExprStmt\n");
        depth++;
        v.body->acceptVisitor(*this);
        depth--;
    }

    void visit(const UnaryExpr& v) override {
        printPrefix();
        fmt::print("UnaryExpr\n");
        depth++;
        printToken(v.op);
        v.expr->acceptVisitor(*this);
        depth--;
    }
    void visit(const BinaryExpr& v) override {
        printPrefix();
        fmt::print("BinaryExpr\n");
        depth++;
        v.lhs->acceptVisitor(*this);
        printToken(v.op);
        v.rhs->acceptVisitor(*this);
        depth--;
    }
    void visit(const CompoundExpr& v) override {
        printPrefix();
        fmt::print("CompoundExpr\n");
        depth++;
        for(auto& e : v.preface)
            std::visit([this](const auto& e) { e->acceptVisitor(*this); }, e);
        v.last->acceptVisitor(*this);
        depth--;
    }
    void visit(const IfExpr& v) override {
        printPrefix();
        fmt::print("IfExpr\n");
        depth++;
        v.cond->acceptVisitor(*this);
        v.on_true->acceptVisitor(*this);
        v.on_false->acceptVisitor(*this);
        depth--;
    }
    void visit(const ReturnExpr& v) override {
        printPrefix();
        fmt::print("ReturnExpr\n");
        depth++;
        v.result->acceptVisitor(*this);
        depth--;
    }
    void visit(const LiteralExpr& v) override {
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

}