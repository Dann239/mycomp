#include "mycomp/lex.hpp"

#include <catch2/catch_test_macros.hpp>

#include <fmt/core.h>

#include <iostream>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <string_view>
#include <vector>

using enum mycomp::TokenType;
using std::get, std::uint64_t;

std::vector<mycomp::Token> wrap_lex(std::string_view str) {
    try {
        return mycomp::lex(str);
    }
    catch(const mycomp::LexException& e) {
        throw std::runtime_error(e.error);
    }
}

TEST_CASE("Lexer: Just works", "[lex]") {
    auto v = wrap_lex("1 + 2");

    CHECK(v[0].tokenType == NATURAL_NUMBER);
    CHECK(v[1].tokenType == PLUS);
    CHECK(v[2].tokenType == NATURAL_NUMBER);
}

TEST_CASE("Parens", "[lex]") {
    auto v = wrap_lex("1 + (2)");

    CHECK(v[0].tokenType == NATURAL_NUMBER);
    CHECK(get<uint64_t>(v[0].payload) == 1);
    CHECK(v[1].tokenType == PLUS);
    CHECK(v[2].tokenType == LEFT_PAREN);
    CHECK(v[3].tokenType == NATURAL_NUMBER);
    CHECK(get<uint64_t>(v[3].payload) == 2);
    CHECK(v[4].tokenType == RIGHT_PAREN);
}

TEST_CASE("Keywords", "[lex]") {
    auto v = wrap_lex(
            "var fn if else "
            "return true false"
    );

    CHECK(v[0].tokenType == VAR);
    CHECK(v[1].tokenType == FUN);
    CHECK(v[2].tokenType == IF);
    CHECK(v[3].tokenType == ELSE);
    CHECK(v[4].tokenType == RETURN);
    CHECK(v[5].tokenType == TRUE);
    CHECK(v[6].tokenType == FALSE);
}

TEST_CASE("Consequent", "[lex]") {
    auto v = wrap_lex("!true");

    CHECK(v[0].tokenType == NOT);
    CHECK(v[1].tokenType == TRUE);

    v = wrap_lex("1.e-5-1-.2E-2");
    CHECK(v[0].tokenType == REAL_NUMBER);
    CHECK(get<double>(v[0].payload) == 1e-5);
    CHECK(v[1].tokenType == MINUS);
    CHECK(v[2].tokenType == NATURAL_NUMBER);
    CHECK(get<uint64_t>(v[2].payload) == 1);
    CHECK(v[3].tokenType == MINUS);
    CHECK(v[4].tokenType == REAL_NUMBER);
    CHECK(get<double>(v[4].payload) == 0.2e-2);

}

TEST_CASE("Comments", "[lex]") {
    auto v = wrap_lex(
            "/* inline comment // then newline\n"
            "end inline comment //////// */ //\n"
            "// Comment if var a = 1; \n"
            "// One more comment \n"
            "1 // Token then comment \n"  // <--- Token
            "// Comment with no newline");


    // parses to just `1`
    CHECK(v[0].tokenType == NATURAL_NUMBER);

    CHECK_THROWS(wrap_lex("1 2 3 /* comment with no termination\n"));
    CHECK_THROWS(wrap_lex("forgot to start the comment */ a = 1"));
    CHECK_THROWS(wrap_lex("*/"));
    CHECK_THROWS(wrap_lex("/*"));
    CHECK_THROWS(wrap_lex("/*/"));
}

TEST_CASE("Statement", "[lex]") {
    auto v = wrap_lex("var abc = 0;");

    CHECK(v[0].tokenType == VAR);
    CHECK(v[1].tokenType == IDENTIFIER);
    CHECK(v[2].tokenType == ASSIGN);
    CHECK(v[3].tokenType == NATURAL_NUMBER);
    CHECK(v[4].tokenType == SEMICOLON);
}

TEST_CASE("String literal", "[lex]") {
    auto v = wrap_lex("\"Hello world\"");

    CHECK(v[0].tokenType == STRING);

    CHECK_THROWS(wrap_lex("\" Hello wo"));
    CHECK_THROWS(wrap_lex("\" \\ \"")); // escape seqs arent suuported yet
}

TEST_CASE("Funtion declaration args", "[lex]") {
    auto v = wrap_lex("(a1, a_2)");

    CHECK(v[0].tokenType == LEFT_PAREN);
    CHECK(v[1].tokenType == IDENTIFIER);
    CHECK(v[2].tokenType == COMMA);
    CHECK(v[3].tokenType == IDENTIFIER);
    CHECK(v[4].tokenType == RIGHT_PAREN);
}

TEST_CASE("Curly", "[lex]") {
    auto v = wrap_lex("{ }");

    CHECK(v[0].tokenType == LEFT_BRACE);
    CHECK(v[1].tokenType == RIGHT_BRACE);
}

TEST_CASE("Assign vs Equals", "[lex]") {
    auto v = wrap_lex("== = ==");

    CHECK(v[0].tokenType == EQUALS);
    CHECK(v[1].tokenType == ASSIGN);
    CHECK(v[2].tokenType == EQUALS);
}

TEST_CASE("Lexer numbers", "[lex]") {
    constexpr std::string_view zero = "0";
    static_assert(zero.size() == 1);

    auto v = wrap_lex(zero);
    CHECK(get<uint64_t>(v[0].payload) == 0);

    v = wrap_lex("1e5 100500 1.1e-4 0x10 0x10.8 .1 0");
    CHECK(v[0].tokenType == REAL_NUMBER);
    CHECK(get<double>(v[0].payload) == 1.e5);
    CHECK(v[1].tokenType == NATURAL_NUMBER);
    CHECK(get<uint64_t>(v[1].payload) == 100500);
    CHECK(v[2].tokenType == REAL_NUMBER);
    CHECK(get<double>(v[2].payload) == 1.1e-4);
    CHECK(v[3].tokenType == NATURAL_NUMBER);
    CHECK(get<uint64_t>(v[3].payload) == 16);
    CHECK(v[4].tokenType == REAL_NUMBER);
    CHECK(get<double>(v[4].payload) == 16.5);
    CHECK(v[5].tokenType == REAL_NUMBER);
    CHECK(get<double>(v[5].payload) == 0.1);

    CHECK_THROWS(wrap_lex("0777"));
    CHECK_THROWS(wrap_lex("0x"));
    CHECK_THROWS(wrap_lex("1e4.5"));
    CHECK_THROWS(wrap_lex("1000000000000000000000000000"));
}

TEST_CASE("Almost a keyword", "[lex]") {
    auto v = wrap_lex("tru");
    CHECK(v[0].tokenType == IDENTIFIER);
    CHECK(get<std::string>(v[0].payload) == "tru");
}
