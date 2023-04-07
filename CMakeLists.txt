cmake_minimum_required(VERSION 3.24)
project(search_engine VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(MY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(MY_TOOLS_DIR ${CMAKE_SOURCE_DIR}/tools)

#JSON
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(nlohmann_json)

add_subdirectory(src)

#JSON library
target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)
