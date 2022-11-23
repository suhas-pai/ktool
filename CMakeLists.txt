cmake_minimum_required(VERSION 3.1.0)
project(ktool)

#Bring the headers into the project
include_directories(include)

#Can manually add the sources using the set command as follows:
#set(SOURCES src/main.cpp)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(ktool ${SOURCES})
set_property(TARGET ktool PROPERTY CXX_STANDARD 20)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(ktool PROPERTIES OUTPUT_NAME "ktool_debug")
endif()

install(TARGETS ktool DESTINATION ${CMAKE_INSTALL_PREFIX})