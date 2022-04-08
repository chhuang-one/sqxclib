set(SOURCES_STRUCTS
    SqPairs.c
#    SqCollection.c
)

set(HEADERS_STRUCTS
    SqPairs.h
#    SqCollection.h
)

set(SOURCES_CONSOLE
    SqConsole.c
    SqCommand.c
    SqOption.c
)

set(HEADERS_CONSOLE
    SqConsole.h
    SqCommand.h
    SqOption.h
)

set(SOURCES_TEST
    SqdbEmpty.c
    SqxcEmpty.c
    SqxcFile.c
)
set(HEADERS_TEST
    SqdbEmpty.h
    SqxcEmpty.h
    SqxcFile.h
)

set(SQXCSUPPORT_INCLUDE_DIRS
    ${SQXC_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SQXCSUPPORT_LIBRARIES
    sqxcstructs
    sqxconsole
    sqxctest
    ${SQXC_LIBRARIES}
)

# set SQXCSUPPORT_INCLUDE_DIRS to PARENT_SCOPE
set(SQXCSUPPORT_INCLUDE_DIRS
    ${SQXCSUPPORT_INCLUDE_DIRS}
    PARENT_SCOPE
)

# set SQXCSUPPORT_LIBRARIES to PARENT_SCOPE
set(SQXCSUPPORT_LIBRARIES
    ${SQXCSUPPORT_LIBRARIES}
    PARENT_SCOPE
)

# --- sqxc ---
include_directories(${SQXC_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}")
link_libraries(${SQXC_LIBRARIES})

add_library(sqxcstructs
            STATIC
            ${SOURCES_STRUCTS}
            ${HEADERS_STRUCTS}
)

add_library(sqxconsole
            STATIC
            ${SOURCES_CONSOLE}
            ${HEADERS_CONSOLE}
)

add_library(sqxctest
            STATIC
            ${SOURCES_TEST}
            ${HEADERS_TEST}
)
