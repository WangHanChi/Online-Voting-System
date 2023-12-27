project(client)

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

# create the executable
add_executable(client ${SOURCES}
    $<TARGET_OBJECTS:CommonLib>)
target_include_directories(client PRIVATE ${CommonLibDir}/inc)
