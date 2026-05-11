include_directories(../lab2/include)

add_executable(lab3_main
    ../lab2/app/main.cpp
    ../lab2/src/patricia.cpp
)

add_executable(lab3_profile
    ../lab2/benchmark/benchmark.cpp
    ../lab2/src/patricia.cpp
)

target_compile_options(lab3_profile PRIVATE -pg)
target_link_options(lab3_profile PRIVATE -pg)
