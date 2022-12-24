cmake_minimum_required(VERSION 3.1.0)
project(ktool)

#Bring the headers into the project
include_directories(include)

#Can manually add the sources using the set command as follows:
#set(SOURCES src/main.cpp)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(ktool ${SOURCES})
target_compile_options(ktool PUBLIC -Wall -Wextra -pedantic -Wsign-conversion -Wredundant-decls -Wstrict-overflow=5 -Wundef -Wnull-dereference -funsigned-char)

set_property(TARGET ktool PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;
    -header-filter=include;
    -checks=*;)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(ktool PROPERTIES OUTPUT_NAME "ktool_debug")
    target_link_options(ktool PRIVATE "-fsanitize=address" "-fsanitize=undefined")
endif()

install(TARGETS ktool DESTINATION ${CMAKE_INSTALL_PREFIX})
