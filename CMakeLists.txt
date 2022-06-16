cmake_minimum_required(VERSION 3.22)
project(JSON_parser)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.10.0
)
FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_STANDARD 20)
set(TEST_NAME ${PROJECT_NAME}_tests)
add_executable(${TEST_NAME} TEST.cpp)

target_link_libraries(${TEST_NAME} gtest_main)
#target_link_libraries(${TARGET} JSON_library)
#target_include_directories(${TEST_NAME} PUBLIC
#        "${PROJECT_BINARY_DIR}"
#        "${PROJECT_BINARY_DIR}/JSON_library")
add_subdirectory(JSON_library)

enable_testing()
include(GoogleTest)
gtest_discover_tests(${TEST_NAME})