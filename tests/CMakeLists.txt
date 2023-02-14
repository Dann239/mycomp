project(tests)

add_executable(tests
    scanner_tests.cpp
    lex_tests.cpp
)

target_link_libraries(tests PRIVATE mycomp Catch2::Catch2WithMain)
