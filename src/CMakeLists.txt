cmake_minimum_required(VERSION 3.24)

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

add_executable(search_engine main.cpp
        #${MY_TOOLS_DIR}/fileConvertor.h
        ${MY_INCLUDE_DIR}/testSuits.h testSuits.cpp
        ${MY_INCLUDE_DIR}/converterJson.h converterJson.cpp
        ${MY_INCLUDE_DIR}/invertedIndex.h invertedIndex.cpp
        ${MY_INCLUDE_DIR}/searchServer.h searchServer.cpp)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(search_engine PUBLIC ${MY_INCLUDE_DIR})
include_directories(search_engine PUBLIC ${MY_TOOLS_DIR})

# Googletest library
target_link_libraries(search_engine PRIVATE gtest_main)
include(GoogleTest)
gtest_discover_tests(search_engine)
##
