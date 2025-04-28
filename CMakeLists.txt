cmake_minimum_required(VERSION 3.14)
project(LBNLCPPCommon LANGUAGES CXX)

# Apply the runtime library setting to the main package
if (MSVC)
    add_compile_options("/FC")  # Show full paths when compiling
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Create a header-only library
add_library(LBNLCPPCommon INTERFACE)

# Set include directory for this library
target_include_directories(LBNLCPPCommon INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Require C++20 because of ranges and concepts
target_compile_features(LBNLCPPCommon INTERFACE cxx_std_20)

# Only add GoogleTest if this is the top-level project
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    include(CTest) # Enables testing in CMake
    enable_testing()

    # Fetch GoogleTest
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.16.0
    )

    # Force GoogleTest to use the same runtime library as the project
    if (MSVC)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()

    FetchContent_MakeAvailable(googletest)

    # Collect all .cxx files in the tst directory
    file(GLOB TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tst/*.cxx)

    # Add the test executable
    add_executable(LBNLCPPCommonTests ${TEST_SOURCES})

    # Add include directories for GoogleTest
    target_include_directories(LBNLCPPCommonTests PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)

    # Require C++20 for the test target
    target_compile_features(LBNLCPPCommonTests PRIVATE cxx_std_20)

    # Apply the runtime library setting to the test target
    if (MSVC)
        target_compile_options(LBNLCPPCommonTests PRIVATE /MD$<$<CONFIG:Debug>:d>)
    endif()

    target_link_libraries(LBNLCPPCommonTests PRIVATE gtest_main)

    add_test(NAME LBNLCPPCommonTest COMMAND LBNLCPPCommonTests)
endif()