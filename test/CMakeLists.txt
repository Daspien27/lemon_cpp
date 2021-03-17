cmake_minimum_required(VERSION 3.15)

# ----- Gtest specific config -----
# Reference: https://github.com/google/googletest/blob/master/googletest/README.md

option(BUILD_TESTING "Build test targets" OFF)
if(BUILD_TESTING)
include(GoogleTest)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# ----- Project specific config starts here -----
# Locate relevant executables and set definitions
get_property(LEMPAR_PATH GLOBAL PROPERTY LEMPAR_PATH)
set(SQLITE_GRAMMAR_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data/sqlite_parse.y)
cmake_policy(PUSH)
cmake_policy(SET CMP0026 OLD)
get_property(LEMON_ORIGINAL_EXECUTABLE_PATH TARGET lemon_original PROPERTY LOCATION)
get_property(LEMON_CPP_EXECUTABLE_PATH TARGET lemon_cpp PROPERTY LOCATION)
cmake_policy(POP)
set(LEMON_ORIGINAL_OUT_DIR ${CMAKE_BINARY_DIR}/generatedOriginalOut)
set(LEMON_CPP_OUT_DIR ${CMAKE_BINARY_DIR}/generatedCppOut)
file(MAKE_DIRECTORY ${LEMON_ORIGINAL_OUT_DIR})
file(MAKE_DIRECTORY ${LEMON_CPP_OUT_DIR})
add_definitions(-DLEMPAR_PATH="${LEMPAR_PATH}"
                -DSQLITE_GRAMMAR_PATH="${SQLITE_GRAMMAR_PATH}"
                -DLEMON_ORIGINAL_EXECUTABLE_PATH="${LEMON_ORIGINAL_EXECUTABLE_PATH}"
                -DLEMON_CPP_EXECUTABLE_PATH="${LEMON_CPP_EXECUTABLE_PATH}"
                -DLEMON_ORIGINAL_OUT_DIR="${LEMON_ORIGINAL_OUT_DIR}"
                -DLEMON_CPP_OUT_DIR="${LEMON_CPP_OUT_DIR}")

# Setup tests target
add_executable(lemon_tests "")
add_subdirectory(regression)
target_link_libraries(lemon_tests gtest_main)
gtest_discover_tests(lemon_tests)
ENDIF(BUILD_TESTING)