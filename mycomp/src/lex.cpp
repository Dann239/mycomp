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

enum class ServiceTokenType {
    ONELINE_COMMENT_START,
    INLINE_COMMENT_START,
    INLINE_COMMENT_END,
    DOUBLE_QUOTE,
    NOT_A_SERVICE_TOKEN
};

static constexpr std::pair<std::string_view, ServiceTokenType> service_tokens[] = {
    {"//", ServiceTokenType::ONELINE_COMMENT_START},
    {"/*", ServiceTokenType::INLINE_COMMENT_START},
    {"*/", ServiceTokenType::INLINE_COMMENT_END},
    {"\"", ServiceTokenType::DOUBLE_QUOTE}
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

static constexpr auto size_comparator = [](auto l, auto r) {
    return l.first.size() > r.first.size();
};
static_assert(std::ranges::is_sorted(service_tokens, size_comparator));
static_assert(std::ranges::is_sorted(special_tokens, size_comparator));

std::vector<Token> lex(std::string_view code) {
    auto parse_number = [code](std::size_t i) -> Token {
        bool is_base16 = false;
        bool has_point = false;
        bool has_exponent = false;

        Token res{.begin_pos = i};

        if(code[i] == '0') {
            if(i == code.size() - 1) {
                return Token{
                    .tokenType=NATURAL_NUMBER,
                    .begin_pos=i,
                    .end_pos=i + 1,
                    .payload=(std::uint64_t)0
                };
            }
            else {
                if(std::isdigit(code[i + 1]))
                    throw LexError{
                        .begin_pos=i,
                        .end_pos=i + 1,
                        .error="Leading zero in a number"
                    };

                if(code[i + 1] == 'x' || code[i + 1] == 'X') {
                    is_base16 = true;
                    i += 2; // skip 0x
                }
            }
        }
        auto i_start = i;
        for(; i != code.size(); i++) {
            char c = code[i];
            if(c == '.') {
                if(has_point || has_exponent)
                    throw LexError{
                        .begin_pos=i,
                        .end_pos=i+1,
                        .error="Unexpected point in a number!"
                    };
                else
                    has_point = true;
            }
            else if(c == 'e' || c == 'E') {
                if(has_exponent)
                    throw LexError{
                        .begin_pos=i,
                        .end_pos=i+1,
                        .error="Unexpected E in a number!"
                    };
                else
                    has_exponent = true;
            }
            else if(c == '-' && has_exponent && (code[i - 1] == 'e' || code[i - 1] == 'E')) {
                continue;
            }
            else if(std::isspace(c) || std::ispunct(c))
                break;
            else if(!std::isdigit(c))
                throw LexError{
                    .begin_pos=i,
                    .end_pos=i+1,
                    .error="Unexpected character in a number!"
                };
        }

        bool is_float = has_point || has_exponent;
        res.tokenType = is_float ? REAL_NUMBER : NATURAL_NUMBER;
        res.end_pos = i;

        auto check_fc_result = [&](std::from_chars_result r) {
            auto [last, errc] = r;
            if(errc != std::errc{})
                throw LexError{
                    .begin_pos=res.begin_pos,
                    .end_pos=i,
                    .error=fmt::format(
                        "Error in from_chars while parsing a number: {}",
                        std::make_error_code(errc).message()
                    )
                };
            if(last != &code[i])
                throw LexError{
                    .begin_pos=res.begin_pos,
                    .end_pos=i,
                    .error="Number could not be fully parsed!"
                };
        };

        if(is_float) {
            double val = {};
            check_fc_result(std::from_chars(
                &code[i_start],
                &code[i],
                val,
                is_base16 ? std::chars_format::hex : std::chars_format::general
            ));
            res.payload = val;
        }
        else {
            std::uint64_t val = {};
            check_fc_result(std::from_chars(
                &code[i_start],
                &code[i],
                val,
                is_base16 ? 16 : 10
            ));
            res.payload = val;
        }
        return res;
    };

    auto parse_word = [code](std::size_t i) -> Token {
        constexpr std::span keyword_span = keywords;
        static const std::unordered_map<std::string_view, TokenType> keyword_map{keyword_span.begin(), keyword_span.end()};
        auto i_start = i;
        while(i != code.size() && (std::isalnum(code[i]) || code[i] == '_'))
            i++;
        auto word = std::string_view{&code[i_start], &code[i]};
        if(auto iter = keyword_map.find(word); iter != keyword_map.end())
            return Token{
                .tokenType=iter->second,
                .begin_pos=i_start,
                .end_pos=i
            };
        else
            return Token{
                .tokenType=IDENTIFIER,
                .begin_pos=i_start,
                .end_pos=i,
                .payload=std::string{word}
            };
    };

    auto check_for_service_token = [code](std::size_t i) -> ServiceTokenType {
        for(auto [str, tok] : service_tokens)
            if(code.substr(i).starts_with(str))
                return tok;
        return ServiceTokenType::NOT_A_SERVICE_TOKEN;
    };

    auto parse_special_token = [code](std::size_t i) -> Token {
        for(auto [str, tok] : special_tokens)
            if(code.substr(i).starts_with(str))
                return Token{
                    .tokenType=tok,
                    .begin_pos=i,
                    .end_pos=i + str.size()
                };

        throw LexError{
            .begin_pos=i,
            .end_pos=i+1,
            .error="Unexpected sequence of special characters"
        };
    };

    std::vector<Token> res;
    for(std::size_t i = 0; i != code.size();) {
        char c = code[i];

        if(std::isspace(c))
            i++;
        else if(std::isdigit(c)) {
            res.push_back(parse_number(i));
            i = res.back().end_pos;
        }
        else if(i != code.size() - 1 && c == '.' && std::isdigit(code[i + 1])) {
            res.push_back(parse_number(i));
            i = res.back().end_pos;
        }
        else if(std::isalpha(c) || c == '_') {
            res.push_back(parse_word(i));
            i = res.back().end_pos;
        }
        else if(std::ispunct(c))
            switch(check_for_service_token(i)) {
            case ServiceTokenType::ONELINE_COMMENT_START: {
                while(i != code.size() && code[i] != '\n')
                    i++;
                if(i != code.size())
                    i++; // skip the '\n'
                break;
            }
            case ServiceTokenType::INLINE_COMMENT_START: {
                auto i_start = i;
                i += 2; // skip the /*
                while(i < code.size() - 1 && code.substr(i, 2) != "*/")
                    i++;
                if(i >= code.size() - 1)
                    throw LexError{
                        .begin_pos=i_start,
                        .end_pos=i_start + 2,
                        .error="Inline comment not terminated!"
                    };
                i += 2; // skip the "*/"
                break;
            }
            case ServiceTokenType::INLINE_COMMENT_END: {
                throw LexError{
                    .begin_pos=i,
                    .end_pos=i + 2,
                    .error="Unexpected inline comment terminator encountered!"
                };
                break;
            }
            case ServiceTokenType::DOUBLE_QUOTE: {
                auto i_start = i;
                i++; // skip the '"'
                while(i != code.size() && code[i] != '"') {
                    if(code[i] == '\\')
                        throw LexError{
                            .begin_pos=i,
                            .end_pos=i+1,
                            .error="Escape sequences inside strings are not supported yet!"
                        };
                    i++;
                }
                if(i == code.size())
                    throw LexError{
                        .begin_pos=i_start,
                        .end_pos=i_start + 1,
                        .error="String literal is not terminated!"
                    };
                else
                    i++; // skip the '"'

                res.push_back(Token{
                    .tokenType=STRING,
                    .begin_pos=i_start,
                    .end_pos=i,
                    .payload=std::string(code.substr(i_start + 1, i - 1))
                });

                break;
            }
            case ServiceTokenType::NOT_A_SERVICE_TOKEN: {
                res.push_back(parse_special_token(i));
                i = res.back().end_pos;
                break;
            }
            }
        else
            throw LexError{
                .begin_pos=i,
                .end_pos=i + 1,
                .error="Unrecognized symbol!"
            };
    }
    return res;
}

}