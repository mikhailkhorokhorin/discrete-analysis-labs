lab_library(lab1_core
    SOURCES
        src/pair.cpp
        src/radix_sort.cpp
)

lab_executable(lab1_main
    SOURCES app/main.cpp
    LINK lab1_core
)

lab_executable(lab1_solution
    SOURCES solution.cpp
)

lab_executable(lab1_benchmark
    SOURCES benchmark/benchmark.cpp
    LINK lab1_core
)

lab_tests(lab1_tests
    SOURCES
        tests/vector.cpp
        tests/pair.cpp
        tests/radix_sort.cpp
        tests/e2e.cpp
    LINK lab1_core
    BINARIES lab1_main lab1_solution
)
