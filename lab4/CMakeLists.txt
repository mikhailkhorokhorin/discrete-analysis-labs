include_directories(include)

add_library(lab4_core
    src/kmp.cpp
)

add_executable(lab4_main
    app/main.cpp
)

add_executable(lab4_benchmark
    benchmark/benchmark.cpp
)

target_link_libraries(lab4_main PRIVATE lab4_core)
target_link_libraries(lab4_benchmark PRIVATE lab4_core)
