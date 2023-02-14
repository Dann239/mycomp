#include "mycomp/scanner.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Scanner cornercases", "[scanner]") {
    auto amogus = mycomp::Scanner("amogus");
    CHECK(amogus.curr() == 'a');
    CHECK(amogus.peek() == 'm');
    CHECK(amogus.peek(2) == 'o');
    CHECK(amogus.substr(1) == "a");

    amogus.advance();
    CHECK(amogus.curr() == 'm');
    CHECK(amogus.peek() == 'o');
    CHECK(amogus.peek(2) == 'g');
    CHECK(amogus.substr(100) == "mogus");

    CHECK_NOTHROW(amogus.advance(100500));
    CHECK_NOTHROW(amogus.substr());
}
