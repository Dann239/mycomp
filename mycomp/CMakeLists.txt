add_library(mycomp
    src/lex.cpp
    src/utils/token_to_string.cpp
)

target_include_directories(mycomp PUBLIC include)
target_link_libraries(mycomp PUBLIC fmt)
target_link_libraries(mycomp PRIVATE magic_enum)
