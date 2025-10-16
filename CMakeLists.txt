cmake_minimum_required(VERSION 3.20)
project(ThreadByteTree LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_library(threadbytetree STATIC
        src/ThreadByteTree.cpp
        src/comparator.cpp
        src/skiplist.cpp
)

target_include_directories(threadbytetree
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (MSVC)
    target_compile_options(threadbytetree PRIVATE /W4 /permissive- /EHsc)
else()
    target_compile_options(threadbytetree PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
endif()

target_link_libraries(threadbytetree PUBLIC Threads::Threads)

add_executable(threadbytetree_tests_main
        tests/main.cpp
)

add_executable(threadbytetree_tests_comparator
        tests/comparator.cpp
)

target_link_libraries(threadbytetree_tests_main
        PRIVATE threadbytetree Threads::Threads
)

target_link_libraries(threadbytetree_tests_comparator
        PRIVATE threadbytetree Threads::Threads
)

include(CTest)
if (BUILD_TESTING)
    add_test(NAME main COMMAND threadbytetree_tests_main)
    add_test(NAME comparator COMMAND threadbytetree_tests_comparator)
endif()