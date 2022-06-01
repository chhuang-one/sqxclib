# helpers sources & headers
set(SOURCES_HELPERS
    SqStr.c
)

set(HEADERS_HELPERS
    SqStr.h
)

# data structs sources & headers
set(SOURCES_STRUCTS
    SqPairs.c
    SqRow.c
#    SqCollection.c
)

set(HEADERS_STRUCTS
    SqPairs.h
    SqRow.h
#    SqCollection.h
)

# console sources & headers
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

# test sources & headers
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

# --- sqxcsupport ---
set(SQXCSUPPORT_INCLUDE_DIRS
    ${SQXC_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SQXCSUPPORT_LIBRARIES
    sqxcsupport
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

# library
include_directories(${SQXC_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}")
link_libraries(${SQXC_LIBRARIES})

add_library(sqxcsupport
            STATIC
            ${SOURCES_HELPERS}
            ${SOURCES_STRUCTS}
            ${SOURCES_CONSOLE}
            ${SOURCES_TEST}

            ${HEADERS_HELPERS}
            ${HEADERS_STRUCTS}
            ${HEADERS_CONSOLE}
            ${HEADERS_TEST}
)
