lab_library(lab4_core
    SOURCES
        src/kmp.cpp
        src/search.cpp
)

lab_executable(lab4_main
    SOURCES app/main.cpp
    LINK lab4_core
)

lab_executable(lab4_solution
    SOURCES solution.cpp
)

lab_executable(lab4_benchmark
    SOURCES benchmark/benchmark.cpp
    LINK lab4_core
)

lab_tests(lab4_tests
    SOURCES
        tests/kmp.cpp
        tests/search.cpp
        tests/e2e.cpp
    LINK lab4_core
    BINARIES lab4_main lab4_solution
)
