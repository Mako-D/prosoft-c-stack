cmake_minimum_required(VERSION 3.20)
project(cstack)


enable_testing()

option(WITH_TEST "Build test (GTest library required)" ON)

include(FetchContent)

set(CMAKE_C_STANDARD_REQUIRED ON)

# build library
add_library(cstack STATIC cstack.c)

target_compile_features(cstack PUBLIC c_std_11)

# build test
if (WITH_TEST)
    enable_testing()
    add_executable(cstack_test test.cpp)
    target_link_libraries(cstack_test cstack)
    add_test(NAME cstack_test COMMAND cstack_test)

    # link with googletest
    find_package(GTest)
    if (NOT ${GTest_FOUND})
        include(cmake/gtest.cmake)
    endif()
    target_link_libraries(cstack_test
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        GTest::gmock_main
    )
endif()

# compiler-specific project settings

target_compile_options(cstack PRIVATE -Wall -Wextra -Wformat -Wformat-security -Werror -Wno-error=missing-braces -Wno-error=implicit-function-declaration)

set(SANITIZERS_FLAGS -fsanitize=undefined -fsanitize=address)

if (WITH_TEST)
    target_compile_options(cstack_test PRIVATE ${SANITIZERS_FLAGS})
    target_link_options(cstack_test PRIVATE ${SANITIZERS_FLAGS})
endif()