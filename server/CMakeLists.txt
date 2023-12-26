project(server)

# server source files
set(SOURCES
    main-server.c
    handler.c
    login.c
    syslog.c
)

# create the executable
add_executable(server ${SOURCES})
