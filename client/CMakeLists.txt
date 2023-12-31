cmake_minimum_required(VERSION 3.12)
project(client)

# set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# check if CMAKE_BUILD_TYPE is Debug
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

# client source files
set(SOURCES
    main-client.c
    command.c
    user_login.c
)

set(CommonLibDir ${CMAKE_SOURCE_DIR}/../common)

add_library(CommonLib OBJECT
      ${CommonLibDir}/src/unix_utils.c)
target_include_directories(CommonLib PRIVATE ${CommonLibDir}/inc)

add_library(ProtoLib OBJECT
      ${CommonLibDir}/src/proto_lib.c)
target_include_directories(ProtoLib PRIVATE ${CommonLibDir}/inc)

add_library(StrLib OBJECT
      ${CommonLibDir}/src/string_utils.c)
target_include_directories(StrLib PRIVATE ${CommonLibDir}/inc)

# create the executable
add_executable(client ${SOURCES}
    $<TARGET_OBJECTS:CommonLib>
    $<TARGET_OBJECTS:ProtoLib>
    $<TARGET_OBJECTS:StrLib>)

target_include_directories(client PRIVATE ${CommonLibDir}/inc)

# clang-format
add_custom_target(format
    COMMAND clang-format -i ${CMAKE_SOURCE_DIR}/*.c ${CMAKE_SOURCE_DIR}/*.h
    COMMAND clang-format -i ${CommonLibDir}/src/* ${CommonLibDir}/inc/*
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# clean all
add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/build
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
