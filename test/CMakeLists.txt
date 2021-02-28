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
add_executable(lemon_tests "")
add_subdirectory(regression)
get_property(LEMPAR_PATH GLOBAL PROPERTY LEMPAR_PATH)
set(SQLITE_GRAMMAR_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data/sqlite_parse.y)
add_custom_target(
        prepare_regression_tests
        COMMAND rm -rf lemon_original_out lemon_cpp_out
        COMMAND mkdir lemon_original_out && $<TARGET_FILE:lemon_cpp> -T"${LEMPAR_PATH}" -d"lemon_original_out" ${SQLITE_GRAMMAR_PATH}
        COMMAND mkdir lemon_cpp_out && $<TARGET_FILE:lemon_original> -T"${LEMPAR_PATH}" -d"lemon_cpp_out" ${SQLITE_GRAMMAR_PATH}
)
add_dependencies(lemon_tests prepare_regression_tests)
target_link_libraries(lemon_tests gtest_main)
gtest_discover_tests(lemon_tests)
ENDIF(BUILD_TESTING)