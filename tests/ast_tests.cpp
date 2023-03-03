#include "mycomp/ast.hpp"
#include "mycomp/lex.hpp"
#include "mycomp/utils/print_ast.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <concepts>
#include <memory>
#include <type_traits>

/*
TODO:
!(a+b)
!!a
*/

template<typename T, std::same_as<T>... Ts>
static auto make_vector(T&& elem, Ts&&... elems) {
    // because initializer lists are broken
    std::vector<std::remove_cvref_t<T>> res;
    res.push_back(std::forward<T>(elem));
    ((res.push_back(std::forward<T>(elems))),...);
    return res;
}

TEST_CASE("AST: try printing", "[ast]") {
    using namespace mycomp;
    using enum TokenType;
    using enum AstNodeType;

    auto module = make_ast_node(AstNodeBody<MODULE>{.decls = make_vector<DeclPtr>(
        make_ast_node(AstNodeBody<VARIABLE_DECL>{
            .name = Token{IDENTIFIER, 0, 0, std::string("foo")},
            .type = make_ast_node(AstNodeBody<PRIMITIVE_TYPE>{.body = Token{IDENTIFIER, 0, 0, std::string("int")}}),
            .value = make_ast_node(AstNodeBody<BINARY_EXPR>{
                .op = Token{PLUS, 0, 0},
                .lhs = make_ast_node(AstNodeBody<LITERAL_EXPR>{.body = Token{NATURAL_NUMBER, 0, 0, std::uint64_t{1}}}),
                .rhs = make_ast_node(AstNodeBody<LITERAL_EXPR>{.body = Token{NATURAL_NUMBER, 0, 0, std::uint64_t{2}}})
            })
        })
    )});

    auto printer = make_ast_printer();
    CHECK_NOTHROW(module->acceptVisitor(*printer));
}
