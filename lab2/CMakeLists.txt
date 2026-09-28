lab_library(lab2_core
    SOURCES
        src/dictionary.cpp
        src/patricia_trie.cpp
)

lab_executable(lab2_main
    SOURCES app/main.cpp
    LINK lab2_core
)

lab_executable(lab2_solution
    SOURCES solution.cpp
)

lab_executable(lab2_benchmark
    SOURCES benchmark/benchmark.cpp
    LINK lab2_core
)

lab_tests(lab2_tests
    SOURCES
        tests/patricia_trie.cpp
        tests/dictionary.cpp
        tests/e2e.cpp
    LINK lab2_core
    BINARIES lab2_main lab2_solution
)
