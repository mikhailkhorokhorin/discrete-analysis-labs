lab_library(lab5_core
    SOURCES
        src/lcs.cpp
        src/suffix_tree.cpp
)

lab_executable(lab5_main
    SOURCES app/main.cpp
    LINK lab5_core
)

lab_executable(lab5_solution
    SOURCES solution.cpp
)

lab_executable(lab5_benchmark
    SOURCES benchmark/benchmark.cpp
    LINK lab5_core
)

lab_tests(lab5_tests
    SOURCES
        tests/suffix_tree.cpp
        tests/lcs.cpp
        tests/e2e.cpp
    LINK lab5_core
    BINARIES lab5_main lab5_solution
)
