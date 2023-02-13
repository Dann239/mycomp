add_library(mycomp
    src/lex.cpp
)

target_include_directories(mycomp PUBLIC include)
target_link_libraries(mycomp PUBLIC fmt)
