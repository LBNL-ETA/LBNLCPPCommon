cmake_minimum_required(VERSION 3.14)
project(LBNLCPPCommon LANGUAGES CXX)

# Create a header-only library
add_library(LBNLCPPCommon INTERFACE)

# Set include directory for this library
target_include_directories(LBNLCPPCommon INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Require C++20 because of ranges and concepts
target_compile_features(LBNLCPPCommon INTERFACE cxx_std_20)
