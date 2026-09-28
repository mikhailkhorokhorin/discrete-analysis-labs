set(LAB2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../lab2")
set(LAB2_CORE_SOURCES
    "${LAB2_DIR}/src/dictionary.cpp"
    "${LAB2_DIR}/src/patricia_trie.cpp"
)

lab_executable(lab3_main
    SOURCES "${LAB2_DIR}/app/main.cpp" ${LAB2_CORE_SOURCES}
)
target_include_directories(lab3_main PRIVATE "${LAB2_DIR}/include")

lab_executable(lab3_profile
    SOURCES "${LAB2_DIR}/benchmark/benchmark.cpp" ${LAB2_CORE_SOURCES}
)
target_include_directories(lab3_profile PRIVATE "${LAB2_DIR}/include")
target_compile_options(lab3_profile PRIVATE -pg)
target_link_options(lab3_profile PRIVATE -pg)
