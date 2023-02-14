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
using std::uint64_t;

TEST_CASE("Lexer: Just works", "[lex]") {
    auto l = mycomp::Lexer("1 + 2");

    CHECK(l.next().value() == mycomp::Token{NATURAL_NUMBER, 0, 1, uint64_t{1}});
    CHECK(l.next().value() == mycomp::Token{PLUS, 2, 3});
    CHECK(l.next().value() == mycomp::Token{NATURAL_NUMBER, 4, 5, uint64_t{2}});
    CHECK(!l.next().has_value());
}

TEST_CASE("Parens", "[lex]") {
    auto l = mycomp::Lexer("1 + (2)");

    CHECK(l.next().value() == mycomp::Token{NATURAL_NUMBER, 0, 1, uint64_t{1}});
    CHECK(l.next().value() == mycomp::Token{PLUS, 2, 3});
    CHECK(l.next().value() == mycomp::Token{LEFT_PAREN, 4, 5});
    CHECK(l.next().value() == mycomp::Token{NATURAL_NUMBER, 5, 6, uint64_t{2}});
    CHECK(l.next().value() == mycomp::Token{RIGHT_PAREN, 6, 7});
    CHECK(!l.next().has_value());
}

TEST_CASE("Keywords", "[lex]") {
    auto l = mycomp::Lexer(
            "var fn if else "
            "return true false"
    );

    CHECK(l.next().value() == mycomp::Token{VAR, 0, 3});
    CHECK(l.next().value() == mycomp::Token{FUN, 4, 6});
    CHECK(l.next().value() == mycomp::Token{IF, 7, 9});
    CHECK(l.next().value() == mycomp::Token{ELSE, 10, 14});
    CHECK(l.next().value() == mycomp::Token{RETURN, 15, 21});
    CHECK(l.next().value() == mycomp::Token{TRUE, 22, 26});
    CHECK(l.next().value() == mycomp::Token{FALSE, 27, 32});
    CHECK(!l.next().has_value());
}

TEST_CASE("Consequent", "[lex]") {
    auto l = mycomp::Lexer("!true");

    CHECK(l.next().value() == mycomp::Token{NOT, 0, 1});
    CHECK(l.next().value() == mycomp::Token{TRUE, 1, 5});
    CHECK(!l.next().has_value());

    l = mycomp::Lexer("1.e-5-1-.2E-2");
    CHECK(l.next().value() == mycomp::Token{REAL_NUMBER, 0, 5, double{1e-5}});
    CHECK(l.next().value() == mycomp::Token{MINUS, 5, 6});
    CHECK(l.next().value() == mycomp::Token{NATURAL_NUMBER, 6, 7, uint64_t{1}});
    CHECK(l.next().value() == mycomp::Token{MINUS, 7, 8});
    CHECK(l.next().value() == mycomp::Token{REAL_NUMBER, 8, 13, double{0.2e-2}});
    CHECK(!l.next().has_value());
}

TEST_CASE("Comments", "[lex]") {
    auto l = mycomp::Lexer(
            "/* inline comment // then newline\n"
            "end inline comment //////// */ //\n"
            "// Comment if var a = 1; \n"
            "// One more comment \n"
            "1 // Token then comment \n"  // <--- Token
            "// Comment with no newline");


    // parses to just `1`
    CHECK(l.next().value().tokenType == NATURAL_NUMBER);
    CHECK(!l.next().has_value());

    CHECK_THROWS(mycomp::lex("1 2 3 /* comment with no termination\n"));
    CHECK_THROWS(mycomp::lex("forgot to start the comment */ a = 1"));
    CHECK_THROWS(mycomp::lex("*/"));
    CHECK_THROWS(mycomp::lex("/*"));
    CHECK_THROWS(mycomp::lex("/*/"));
}

TEST_CASE("Statement", "[lex]") {
    auto l = mycomp::Lexer("var abc = 0;");

    CHECK(l.next().value() == mycomp::Token{VAR, 0, 3});
    CHECK(l.next().value() == mycomp::Token{IDENTIFIER, 4, 7, std::string("abc")});
    CHECK(l.next().value() == mycomp::Token{ASSIGN, 8, 9});
    CHECK(l.next().value() == mycomp::Token{NATURAL_NUMBER, 10, 11, uint64_t{0}});
    CHECK(l.next().value() == mycomp::Token{SEMICOLON, 11, 12});
    CHECK(!l.next().has_value());
}

TEST_CASE("String literal", "[lex]") {
    auto l = mycomp::Lexer("\"Hello world\"");

    CHECK(l.next().value() == mycomp::Token{STRING, 0, 13, std::string("Hello world")});
    CHECK(!l.next().has_value());

    CHECK_THROWS(mycomp::lex("\" Hello wo"));
    CHECK_THROWS(mycomp::lex("\" \\ \"")); // escape seqs arent suuported yet
}

TEST_CASE("Funtion declaration args", "[lex]") {
    auto l = mycomp::Lexer("(a1, a_2)");

    CHECK(l.next().value() == mycomp::Token{LEFT_PAREN, 0, 1});
    CHECK(l.next().value() == mycomp::Token{IDENTIFIER, 1, 3, std::string("a1")});
    CHECK(l.next().value() == mycomp::Token{COMMA, 3, 4});
    CHECK(l.next().value() == mycomp::Token{IDENTIFIER, 5, 8, std::string("a_2")});
    CHECK(l.next().value() == mycomp::Token{RIGHT_PAREN, 8, 9});
    CHECK(!l.next().has_value());
}

TEST_CASE("Curly", "[lex]") {
    auto l = mycomp::Lexer("{ }");

    CHECK(l.next().value() == mycomp::Token{LEFT_BRACE, 0, 1});
    CHECK(l.next().value() == mycomp::Token{RIGHT_BRACE, 2, 3});
    CHECK(!l.next().has_value());
}

TEST_CASE("Assign vs Equals", "[lex]") {
    auto l = mycomp::Lexer("== = ==");

    CHECK(l.next().value() == mycomp::Token{EQUALS, 0, 2});
    CHECK(l.next().value() == mycomp::Token{ASSIGN, 3, 4});
    CHECK(l.next().value() == mycomp::Token{EQUALS, 5, 7});
    CHECK(!l.next().has_value());
}

TEST_CASE("Lexer numbers", "[lex]") {
    constexpr std::string_view zero = "0";
    static_assert(zero.size() == 1);

    auto l = mycomp::Lexer(zero);
    CHECK(l.next().value() == mycomp::Token{NATURAL_NUMBER, 0, 1, uint64_t{0}});
    CHECK(!l.next().has_value());

    l = mycomp::Lexer("6");
    CHECK(l.next().value() == mycomp::Token{NATURAL_NUMBER, 0, 1, uint64_t{6}});
    CHECK(!l.next().has_value());


    l = mycomp::Lexer("1e5 100500 1.1e-4 0x10 -0x10.8 .1 0");
    CHECK(l.next().value() == mycomp::Token{REAL_NUMBER, 0, 3, double{1e5}});
    CHECK(l.next().value() == mycomp::Token{NATURAL_NUMBER, 4, 10, uint64_t{100500}});
    CHECK(l.next().value() == mycomp::Token{REAL_NUMBER, 11, 17, double{1.1e-4}});
    CHECK(l.next().value() == mycomp::Token{NATURAL_NUMBER, 18, 22, uint64_t{16}});
    CHECK(l.next().value() == mycomp::Token{MINUS, 23, 24});
    CHECK(l.next().value() == mycomp::Token{REAL_NUMBER, 24, 30, double{16.5}});
    CHECK(l.next().value() == mycomp::Token{REAL_NUMBER, 31, 33, double{0.1}});
    CHECK(l.next().value() == mycomp::Token{NATURAL_NUMBER, 34, 35, uint64_t{0}});
    CHECK(!l.next().has_value());

    CHECK_THROWS(mycomp::lex("0777"));
    CHECK_THROWS(mycomp::lex("0x"));
    CHECK_THROWS(mycomp::lex("1e4.5"));
    CHECK_THROWS(mycomp::lex("1000000000000000000000000000"));
}

TEST_CASE("Almost a keyword", "[lex]") {
    auto l = mycomp::Lexer("tru");
    CHECK(l.next().value() == mycomp::Token{IDENTIFIER, 0, 3, std::string("tru")});
    CHECK(!l.next().has_value());
}
