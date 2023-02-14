#include "mycomp/lex.hpp"

#include <cstdint>
#include <fmt/core.h>

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstddef>
#include <span>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <unordered_map>

namespace mycomp {

using enum TokenType;

static constexpr std::pair<std::string_view, TokenType> keywords[] = {
    {"true", TRUE},
    {"false", FALSE},
    {"fn", FUN},
    {"var", VAR},
    {"if", IF},
    {"else", ELSE},
    {"return", RETURN}
};

static constexpr std::pair<std::string_view, TokenType> special_tokens[] = {
    {"==", EQUALS},
    {"!=", NOT_EQ},
    {"+", PLUS},
    {"-", MINUS},
    {"*", STAR},
    {"/", DIV},
    {"=", ASSIGN},
    {"!", NOT},
    {"<", LESS_THAN},
    {">", GREATER_THAN},
    {"(", LEFT_PAREN},
    {")", RIGHT_PAREN},
    {"{", LEFT_BRACE},
    {"}", RIGHT_BRACE},
    {",", COMMA},
    {";", SEMICOLON}
};

static_assert(std::ranges::is_sorted(
    special_tokens,
    [](auto l, auto r) {
        return l.first.size() > r.first.size();
    }
), "We want our special tokens to come in descending order of size");


Lexer::Lexer(std::string_view code) : s_(code) {}


std::optional<Token> Lexer::next() {
    skipWhitespaceAndComments();
    char c = s_.curr(), c2 = s_.peek();

    if(s_.end())
        return {};
    if(std::isdigit(c))
        return parseNumber();
    if(c == '.' && std::isdigit(c2))
        return parseNumber();
    if(c == '_' || std::isalpha(c))
        return parseWord();
    if(c == '"')
        return parseString();
    if(std::ispunct(c))
        return parseSpecialToken();

    throw LexException{
        .begin_pos=s_.ind(),
        .end_pos=s_.ind() + 1,
        .error="Unrecognized character!"
    };
}


void Lexer::skipWhitespaceAndComments() {
    while(!s_.end()) {
        if(std::isspace(s_.curr()))
            s_.advance();
        else if(s_.substr(2) == "//")
            while(!s_.end() && s_.curr() != '\n')
                s_.advance();
        else if(s_.substr(2) == "/*") {
            auto ind_start = s_.ind();
            s_.advance(2);
            while(s_.substr(2) != "*/") {
                s_.advance();
                if(s_.end())
                    throw LexException{
                        .begin_pos=ind_start,
                        .end_pos=ind_start + 2,
                        .error="Inline comment not terminated!"
                    };
            }
            s_.advance(2);
        } else if(s_.substr(2) == "*/")
            throw LexException{
                .begin_pos=s_.ind(),
                .end_pos=s_.ind() + 2,
                .error="Unexpected inline comment terminator encountered!"
            };
        else
            break;
    }
}


Token Lexer::parseNumber() {
    bool is_base16 = false;
    bool has_point = false;
    bool has_exponent = false;

    auto ind_start = s_.ind();

    if(s_.curr() == '0') {
        char c2 = s_.peek();
        if(std::isdigit(c2))
            throw LexException{
                .begin_pos=ind_start,
                .end_pos=ind_start + 1,
                .error="Leading zero in a number!"
            };

        if(c2 == 'x' || c2 == 'X') {
            is_base16 = true;
            s_.advance(2); // skip 0x
        }
    }

    const auto* start_ptr = s_.raw().data() + s_.ind();
    for(; !s_.end(); s_.advance()) {
        char c = s_.curr();
        if(c == '.') {
            if(has_point || has_exponent)
                throw LexException{
                    .begin_pos=s_.ind(),
                    .end_pos=s_.ind() + 1,
                    .error="Unexpected point in a number!"
                };
            else
                has_point = true;
        }
        else if(c == 'e' || c == 'E') {
            if(has_exponent)
                throw LexException{
                    .begin_pos=s_.ind(),
                    .end_pos=s_.ind() + 1,
                    .error="Unexpected E in a number!"
                };
            else {
                has_exponent = true;
                if(s_.peek() == '-')
                    s_.advance(); // '-' is ok here
            }
        }
        else if(std::isspace(c) || std::ispunct(c))
            break;
        else if(!std::isdigit(c))
            throw LexException{
                .begin_pos=s_.ind(),
                .end_pos=s_.ind() + 1,
                .error=fmt::format("Unexpected character in a number: '{}'!", c)
            };
    }
    const char* end_ptr = s_.raw().data() + s_.ind();

    bool is_float = has_point || has_exponent;

    Token res{
        .tokenType=is_float ? REAL_NUMBER : NATURAL_NUMBER,
        .begin_pos=ind_start,
        .end_pos=s_.ind()
    };

    auto check_fc_result = [&](std::from_chars_result r) {
        auto [last, errc] = r;
        if(errc != std::errc{})
            throw LexException{
                .begin_pos=res.begin_pos,
                .end_pos=res.end_pos,
                .error=fmt::format(
                    "Error in from_chars while parsing a number: {}",
                    std::make_error_code(errc).message()
                )
            };
        if(last != end_ptr)
            throw LexException{
                .begin_pos=res.begin_pos,
                .end_pos=res.end_pos,
                .error="Number could not be fully parsed!"
            };
    };

    if(is_float) {
        double val = {};
        check_fc_result(std::from_chars(
            start_ptr,
            end_ptr,
            val,
            is_base16 ? std::chars_format::hex : std::chars_format::general
        ));
        res.payload = val;
    }
    else {
        std::uint64_t val = {};
        check_fc_result(std::from_chars(
            start_ptr,
            end_ptr,
            val,
            is_base16 ? 16 : 10
        ));
        res.payload = val;
    }
    return res;
}


Token Lexer::parseString() {
    auto ind_start = s_.ind();
    s_.advance(); // skip the '"'
    while(s_.curr() != '"') {
        if(s_.curr() == '\\')
            throw LexException{
                .begin_pos=s_.ind(),
                .end_pos=s_.ind() + 1,
                .error="Escape sequences inside strings are not supported yet!"
            };
        s_.advance();
        if(s_.end())
            throw LexException{
                .begin_pos=ind_start,
                .end_pos=ind_start + 1,
                .error="String literal is not terminated!"
            };
    }
    s_.advance(); // skip the '"'

    return Token{
        .tokenType=STRING,
        .begin_pos=ind_start,
        .end_pos=s_.ind(),
        .payload=std::string(s_.raw().substr(ind_start + 1, (s_.ind() - 1) - (ind_start + 1)))
    };
}


Token Lexer::parseWord() {
    constexpr std::span keyword_span = keywords;
    static const std::unordered_map<std::string_view, TokenType> keyword_map{keyword_span.begin(), keyword_span.end()};
    auto ind_start = s_.ind();
    while(std::isalnum(s_.curr()) || s_.curr() == '_')
        s_.advance();
    auto word = s_.raw().substr(ind_start, s_.ind() - ind_start);
    if(auto iter = keyword_map.find(word); iter != keyword_map.end())
        return Token{
            .tokenType=iter->second,
            .begin_pos=ind_start,
            .end_pos=s_.ind()
        };
    else
        return Token{
            .tokenType=IDENTIFIER,
            .begin_pos=ind_start,
            .end_pos=s_.ind(),
            .payload=std::string{word}
        };
}


Token Lexer::parseSpecialToken() {
    for(auto [str, tok] : special_tokens)
        if(s_.substr(str.size()) == str) {
            s_.advance(str.size());
            return Token{
                .tokenType=tok,
                .begin_pos=s_.ind() - str.size(),
                .end_pos=s_.ind()
            };
        }

    throw LexException{
        .begin_pos=s_.ind(),
        .end_pos=s_.ind() + 1,
        .error="Unexpected sequence of special characters"
    };
}


std::vector<Token> lex(std::string_view code) {
    std::vector<Token> res;
    Lexer lexer(code);
    while(true) {
        auto tok = lexer.next();
        if(!tok.has_value())
            return res;
        res.push_back(*tok);
    }
}

}
