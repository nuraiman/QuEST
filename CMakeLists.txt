# CMake initialisation.
cmake_minimum_required(VERSION 3.1)

# Project name
project(QuEST_Project)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release." FORCE)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CXX_FLAGS ${CXX_FLAGS} -Wall -O0 -std=c++11 -DDEBUG)
else ()
    set(CXX_FLAGS ${CXX_FLAGS} -O3 -std=c++11)
endif ()

add_compile_options(${CXX_FLAGS})

# -----------------------------------------------------------------------------
# ----- QuEST LIBRARY ---------------------------------------------------------
# -----------------------------------------------------------------------------

# Build the QuEST library if the path to libQuEST.so is not specified
if (NOT DEFINED ${QuEST_LIB_PATH})
    # Build libQuEST.so
    set(QuEST_DIR "QuEST" CACHE STRING
        "Name of the directory containing the QuEST library sources. It must be located in the same directory as the root CMakeLists.txt")
    add_subdirectory(${QuEST_DIR})
    set(QuEST_LIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/${QuEST_DIR}")
endif()


# -----------------------------------------------------------------------------
# ----- USER EXECUTABLE -------------------------------------------------------
# -----------------------------------------------------------------------------

# Create user executable
add_executable(demo ${CMAKE_SOURCE_DIR}/examples/tutorial_example.c)

# Link libraries to user executable, including QuEST library
target_link_libraries(demo QuEST m)

# My programs (giulia)
add_executable(gtest ${CMAKE_SOURCE_DIR}/examples_giulia/gtest.cpp)
target_link_libraries(gtest QuEST m)


add_executable(ghad ${CMAKE_SOURCE_DIR}/examples_giulia/ghadamard.cpp)
target_link_libraries(ghad QuEST m)

add_executable(bernstein ${CMAKE_SOURCE_DIR}/examples/bernstein_vazirani_circuit.c)
target_link_libraries(bernstein QuEST m)


# -----------------------------------------------------------------------------
# ----- TESTS -----------------------------------------------------------------
# -----------------------------------------------------------------------------

set(QuEST_TEST_DIR "tests" CACHE STRING
        "Name of the directory containing the QuEST library tests. It must be located in the same directory as the root CMakeLists.txt")
add_subdirectory(${QuEST_TEST_DIR})

