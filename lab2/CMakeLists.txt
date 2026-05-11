include_directories(include)

add_library(lab2_core
    src/patricia.cpp
)

add_executable(lab2_main
    app/main.cpp
)

add_executable(lab2_benchmark
    benchmark/benchmark.cpp
)

target_link_libraries(lab2_main PRIVATE lab2_core)
target_link_libraries(lab2_benchmark PRIVATE lab2_core)
