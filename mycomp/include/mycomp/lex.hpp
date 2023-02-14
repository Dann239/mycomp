#pragma once

#include "scanner.hpp"

#include <cstdint>
#include <exception>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace mycomp {

enum class TokenType {
    NATURAL_NUMBER,
    REAL_NUMBER,
    STRING,
    IDENTIFIER,
    TRUE,
    FALSE,
    PLUS,
    MINUS,
    STAR,
    DIV,
    ASSIGN,
    EQUALS,
    NOT_EQ,
    NOT,
    LESS_THAN,
    GREATER_THAN,
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    SEMICOLON,
    FUN,
    VAR,
    IF,
    ELSE,
    RETURN
};

struct Token {
    TokenType tokenType;
    std::size_t begin_pos, end_pos;
    std::variant<
        std::monostate,
        std::string,
        std::uint64_t,
        double
    > payload;

    bool operator==(const Token&) const = default;
};

struct LexException {
    size_t begin_pos, end_pos;
    std::string error;
};

struct Lexer {
    Lexer(std::string_view code);

    std::optional<Token> next();

private:
    void skipWhitespaceAndComments();
    Token parseNumber();
    Token parseString();
    Token parseWord();
    Token parseSpecialToken();

    Scanner s_;
};

std::vector<Token> lex(std::string_view code);

}
