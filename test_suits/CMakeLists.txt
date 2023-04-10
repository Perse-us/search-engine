cmake_minimum_required(VERSION 3.24)

set(MY_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

# Add GoogleTest fetch
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/7ee260c54921571b18b15049304426fe151c1265.zip
)

# Add GoogleTest directory
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
# GoogleTest: For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# Switch on GoogleTest fetch
FetchContent_MakeAvailable(googletest)
# Switch on testing
enable_testing()
# Googletest to CMake fix (if need)
set (gtest_disable_pthreads on)

# Test configuration
add_executable(test_suits testSuits.cpp
        ${MY_INCLUDE_DIR}/converterJson.h ${MY_SOURCE_DIR}/converterJson.cpp
        ${MY_INCLUDE_DIR}/invertedIndex.h ${MY_SOURCE_DIR}/invertedIndex.cpp
        ${MY_INCLUDE_DIR}/searchServer.h ${MY_SOURCE_DIR}/searchServer.cpp)

include_directories(test_suits PUBLIC ${MY_INCLUDE_DIR})
# Add GoogleTest directory
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Googletest library
target_link_libraries(test_suits PRIVATE gtest_main)
include(GoogleTest)
gtest_discover_tests(test_suits)
