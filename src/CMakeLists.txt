cmake_minimum_required(VERSION 3.24)

# Release configuration
add_executable(search_engine main.cpp
        #${MY_TOOLS_DIR}/fileConvertor.h
        ${MY_INCLUDE_DIR}/converterJson.h converterJson.cpp
        ${MY_INCLUDE_DIR}/invertedIndex.h invertedIndex.cpp
        ${MY_INCLUDE_DIR}/searchServer.h searchServer.cpp)

include_directories(search_engine PUBLIC ${MY_INCLUDE_DIR})
include_directories(search_engine PUBLIC ${MY_TOOLS_DIR})
