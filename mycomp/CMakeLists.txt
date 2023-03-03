add_library(mycomp
    src/lex.cpp
    src/ast.cpp
    src/utils/token_to_string.cpp
    src/utils/print_ast.cpp
)

target_include_directories(mycomp PUBLIC include)
target_link_libraries(mycomp PUBLIC magic_enum)
target_link_libraries(mycomp PRIVATE fmt)
