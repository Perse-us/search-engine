cmake_minimum_required(VERSION 3.24)
project(search_engine VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

#JSON
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(nlohmann_json)

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
#set (gtest_disable_pthreads on)

add_executable(search_engine main.cpp
        fileConvertor.h testSuits.h
        converterJson.h converterJson.cpp
        invertedIndex.h invertedIndex.cpp
        searchServer.h searchServer.cpp)

#JSON library
target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)

# Googletest library
target_link_libraries(search_engine PRIVATE gtest_main)
include(GoogleTest)
gtest_discover_tests(search_engine)
##
