cmake_minimum_required(VERSION 3.14)
project(LBNLCPPCommon VERSION 0.16.0 LANGUAGES CXX)

if (MSVC)
    add_compile_options("/FC")  # Show full paths when compiling
    add_compile_options("/MP")  # Multi-processor (parallel) compilation
endif()

# Local development mode. The `local` preset sets LBNL_LOCAL_SIBLINGS=ON (a cache
# variable, so it cascades into FetchContent sub-builds). When it is on, prefer a
# sibling working copy (../<Name>) of each of THIS project's direct dependencies,
# so the whole dependency graph can be built from local disk during development.
# A dependency whose sibling is not checked out falls back to its declared remote,
# independently. Propagation: a sibling dependency that also honors this flag wires
# up its own siblings in turn. When the flag is off (e.g. the `default` preset),
# nothing here runs and every dependency comes from its remote.
if(LBNL_LOCAL_SIBLINGS)
    foreach(_sibling "googletest=GOOGLETEST")
        string(REPLACE "=" ";" _sibling "${_sibling}")
        list(GET _sibling 0 _sibling_dir)
        list(GET _sibling 1 _sibling_var)
        if(NOT FETCHCONTENT_SOURCE_DIR_${_sibling_var}
           AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../${_sibling_dir}/CMakeLists.txt")
            set(FETCHCONTENT_SOURCE_DIR_${_sibling_var}
                "${CMAKE_CURRENT_SOURCE_DIR}/../${_sibling_dir}")
            message(STATUS "[local] ${_sibling_dir} -> ${FETCHCONTENT_SOURCE_DIR_${_sibling_var}}")
        endif()
    endforeach()
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
