project(client)

# client source files
set(SOURCES
    main-client.c
    command.c
    user_login.c
)

# create the executable
add_executable(client ${SOURCES})
