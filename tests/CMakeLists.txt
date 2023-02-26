project(tests)

add_executable(tests
    scanner_tests.cpp
    lex_tests.cpp
    ast_tests.cpp
)

target_link_libraries(tests PRIVATE mycomp magic_enum Catch2::Catch2WithMain)
