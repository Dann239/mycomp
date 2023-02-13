project(tests)

add_executable(tests
    lex/cases.cpp
)

target_link_libraries(tests PRIVATE mycomp Catch2::Catch2WithMain)
