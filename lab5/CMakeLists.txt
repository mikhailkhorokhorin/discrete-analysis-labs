include_directories(include)

add_library(lab5_core
    src/suffix_tree.cpp
)

add_executable(lab5_main
    app/main.cpp
)

add_executable(lab5_benchmark
    benchmark/benchmark.cpp
)

target_link_libraries(lab5_main PRIVATE lab5_core)
target_link_libraries(lab5_benchmark PRIVATE lab5_core)
