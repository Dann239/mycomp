#include "mycomp/ast.hpp"
#include "mycomp/lex.hpp"
#include "mycomp/utils/print_ast.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <memory>

/*
TODO:
!(a+b)
!!a
*/

TEST_CASE("AST: try printing", "[ast]") {
    using namespace mycomp;
    using enum TokenType;

    auto module = Module{};
    auto decl = std::make_unique<VariableDecl>();

    decl->type.body = Token{IDENTIFIER, 0, 0, std::string("int")};
    decl->name = Token{IDENTIFIER, 0, 0, std::string("foo")};

    auto val = std::make_unique<BinaryExpr>();

    auto lhs = std::make_unique<LiteralExpr>();
    lhs->body = Token{NATURAL_NUMBER, 0, 0, std::uint64_t{1}};
    auto rhs = std::make_unique<LiteralExpr>();
    rhs->body = Token{NATURAL_NUMBER, 0, 0, std::uint64_t{2}};

    val->lhs = std::move(lhs);
    val->op = Token{PLUS, 0, 0};
    val->rhs = std::move(rhs);

    decl->value = std::move(val);

    module.decls.emplace_back(std::move(decl));

    auto printer = AstPrinter{};
    CHECK_NOTHROW(module.acceptVisitor(printer));
}
