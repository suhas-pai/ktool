cmake_minimum_required(VERSION 3.10.0)
project(ktool)

#Bring the headers into the project
include_directories(include)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

#Can manually add the sources using the set command as follows:
#set(SOURCES src/main.cpp)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(ktool ${SOURCES})
set_target_properties (ktool PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED TRUE
    CXX_EXTENSIONS TRUE
)

set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;
    -header-filter=include;
    -checks=*;)

target_compile_options(ktool PUBLIC
    -Wall -Wextra -stdlib=libc++ -pedantic -Wsign-conversion -Wredundant-decls
    -Wstrict-overflow=5 -Wundef -Wnull-dereference -funsigned-char
    -fno-exceptions -Wcast-align -Wfloat-equal -Wredundant-decls
    -Wstrict-overflow=5 -Wundef -Wnull-dereference -funsigned-char -Wformat=2
    -DBUILD_KERNEL -Wno-address-of-packed-member)

target_link_options(ktool PUBLIC -stdlib=libc++)
target_link_options(ktool PUBLIC -fuse-ld=lld)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(ktool PROPERTIES OUTPUT_NAME "ktool_debug")
    target_compile_options(ktool PRIVATE
                           "-fsanitize=address" "-fsanitize=undefined")
    target_link_options(ktool PRIVATE
                        "-fsanitize=address" "-fsanitize=undefined")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(ktool PROPERTIES OUTPUT_NAME "ktool_release")
    target_compile_options(ktool PRIVATE "-O3" "-Wno-unused-parameter" "-Wno-gnu-statement-expression-from-macro-expansion")
    target_link_options(ktool PRIVATE "-O3")
endif()

install(TARGETS ktool DESTINATION ${CMAKE_INSTALL_PREFIX})
