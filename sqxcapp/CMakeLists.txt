# --- sqxcapp ---
set(SOURCES_APP
    SqApp.c
    SqMigration.c
    migrations.c
    migrations-files.c
)

# sqxcapp C++ source files
set(SOURCES_APP_CPP
    migrations-files.cpp
)

set(HEADERS_APP
    SqApp.h
    SqApp-config.h
    SqMigration.h
    migrations.h
)

set(SQXCAPP_INCLUDE_DIRS
    ${SQXC_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SQXCAPP_LIBRARIES
    sqxcapp
    sqxcppapp
    ${SQXC_LIBRARIES}
)

# set SQXCAPP_INCLUDE_DIRS to PARENT_SCOPE
set(SQXCAPP_INCLUDE_DIRS
    ${SQXCAPP_INCLUDE_DIRS}
    PARENT_SCOPE
)

# set SQXCAPP_LIBRARIES to PARENT_SCOPE
set(SQXCAPP_LIBRARIES
    ${SQXCAPP_LIBRARIES}
    PARENT_SCOPE
)

# --- sqxcapptool ---
set(SOURCES_APPTOOL
    SqApp.c
    SqAppTool.c
    SqAppTool-migrations-files.c
    SqMigration.c
    migrations.c
    CommandCommon.c
    CommandMigrate.c
    CommandMake.c
)

# sqxcapptool C++ source files
set(SOURCES_APPTOOL_CPP
    SqAppTool-migrations-files.cpp
)

set(HEADERS_APPTOOL
    SqApp.h
    SqApp-config.h
    SqAppTool.h
    SqMigration.h
    migrations.h
    CommandCommon.h
    CommandMigrate.h
    CommandMake.h
)

set(SQXCAPPTOOL_INCLUDE_DIRS
    ${SQXCSUPPORT_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SQXCAPPTOOL_LIBRARIES
    sqxcapptool
    sqxcppapptool
    ${SQXCSUPPORT_LIBRARIES}
)

# set SQXCAPPTOOL_INCLUDE_DIRS to PARENT_SCOPE
set(SQXCAPPTOOL_INCLUDE_DIRS
    ${SQXCAPPTOOL_INCLUDE_DIRS}
    PARENT_SCOPE
)

# set SQXCAPPTOOL_LIBRARIES to PARENT_SCOPE
set(SQXCAPPTOOL_LIBRARIES
    ${SQXCAPPTOOL_LIBRARIES}
    PARENT_SCOPE
)

# --- sqxctool ---
set(SOURCES
    sqxctool.c
)

set(SOURCES_CPP
    sqxcpptool.cpp
)

# ---
include_directories(${SQXCAPPTOOL_INCLUDE_DIRS})
link_libraries(${SQXC_LIBRARIES})

add_library(sqxcapp
            STATIC
            ${SOURCES_APP}
            ${HEADERS_APP}
)

add_library(sqxcppapp
            STATIC
            ${SOURCES_APP_CPP}
            ${HEADERS_APP}
)

add_library(sqxcapptool
            STATIC
            ${SOURCES_APPTOOL}
            ${HEADERS_APPTOOL}
)

add_library(sqxcppapptool
            STATIC
            ${SOURCES_APPTOOL_CPP}
            ${HEADERS_APPTOOL}
)


add_executable(sqxctool  ${SOURCES})
target_link_libraries(sqxctool  ${SQXCAPPTOOL_LIBRARIES})

add_executable(sqxcpptool  ${SOURCES_CPP})
target_link_libraries(sqxcpptool  ${SQXCAPPTOOL_LIBRARIES})
