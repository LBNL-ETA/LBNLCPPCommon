cmake_minimum_required(VERSION 3.14)
project(LBNLCPPCommon VERSION 0.15.0 LANGUAGES CXX)

if (MSVC)
    add_compile_options("/FC")  # Show full paths when compiling
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

    if (MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()

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
    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tst/*.cxx)

    # Add the test executable
    add_executable(LBNLCPPCommonTests ${TEST_SOURCES})

    target_link_libraries(LBNLCPPCommonTests PRIVATE LBNLCPPCommon gtest_main)

    add_test(NAME LBNLCPPCommonTest COMMAND LBNLCPPCommonTests)
endif()
