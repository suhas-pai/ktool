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

install(TARGETS ktool DESTINATION ${CMAKE_INSTALL_PREFIX})