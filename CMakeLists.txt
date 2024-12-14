cmake_minimum_required(VERSION 3.10)
project(ktool VERSION 0.1.0 LANGUAGES CXX)

include(CTest)
enable_testing()

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(ktool ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CPack)

target_include_directories(ktool PUBLIC include)
set_target_properties (ktool PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED TRUE
  CXX_EXTENSIONS TRUE
)

target_compile_options(ktool PUBLIC -stdlib=libc++ -Iinclude -Wall -Wextra)

target_link_options(ktool PUBLIC -stdlib=libc++)
target_link_options(ktool PUBLIC -fuse-ld=lld)

set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;
    -header-filter=include;
    -checks=*;)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(ktool PRIVATE
                           "-fsanitize=address" "-fsanitize=undefined")
    target_link_options(ktool PRIVATE
                        "-fsanitize=address" "-fsanitize=undefined")
endif()

install_files(. FILES compile_commands.json)
install(TARGETS ktool DESTINATION ${CMAKE_INSTALL_PREFIX})
