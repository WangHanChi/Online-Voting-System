cmake_minimum_required(VERSION 3.12)
project(Online-Voting-System)

# set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# check if CMAKE_BUILD_TYPE is Debug
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

# clang-format
add_custom_target(format
    COMMAND clang-format -i ${CMAKE_SOURCE_DIR}/client/*.c ${CMAKE_SOURCE_DIR}/client/*.h
    COMMAND clang-format -i ${CMAKE_SOURCE_DIR}/server/*.c ${CMAKE_SOURCE_DIR}/server/*.h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# clean all
add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/build
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# test script
add_custom_target(test
    COMMAND sh ${CMAKE_SOURCE_DIR}/script/test.sh
    DEPENDS server client
)

# include subdirectories
add_subdirectory(client)
add_subdirectory(server)
