project(server)

# server source files
set(SOURCES
    main-server.c
    handler.c
    login.c
    syslog.c
)

set(CommonLibDir ${CMAKE_SOURCE_DIR}/../common)

add_library(CommonLib OBJECT
      ${CommonLibDir}/src/unix_utils.c)
target_include_directories(CommonLib PRIVATE ${CommonLibDir}/inc)

# create the executable
add_executable(server ${SOURCES}
    $<TARGET_OBJECTS:CommonLib>)
target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
target_include_directories(server PRIVATE ${CommonLibDir}/inc)
