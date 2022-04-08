# --- sqapp ---
set(SOURCES_APP
    SqApp.c
    SqMigration.c
    migrations.c
    migrations-files.c
)

# sqapp C++ source files
set(SOURCES_APP_CPP
    migrations-files.cpp
)

set(HEADERS_APP
    SqApp.h
    SqApp-config.h
    SqMigration.h
    migrations.h
)

set(SQAPP_INCLUDE_DIRS
    ${SQXC_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SQAPP_LIBRARIES
    sqapp
    sqappcpp
    ${SQXC_LIBRARIES}
)

# set SQAPP_INCLUDE_DIRS to PARENT_SCOPE
set(SQAPP_INCLUDE_DIRS
    ${SQAPP_INCLUDE_DIRS}
    PARENT_SCOPE
)

# set SQAPP_LIBRARIES to PARENT_SCOPE
set(SQAPP_LIBRARIES
    ${SQAPP_LIBRARIES}
    PARENT_SCOPE
)

# --- sqapptool ---
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

# sqapptool C++ source files
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

set(SQAPPTOOL_INCLUDE_DIRS
    ${SQXCSUPPORT_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SQAPPTOOL_LIBRARIES
    sqapptool
    sqapptoolcpp
    ${SQXCSUPPORT_LIBRARIES}
)

# set SQAPPTOOL_INCLUDE_DIRS to PARENT_SCOPE
set(SQAPPTOOL_INCLUDE_DIRS
    ${SQAPPTOOL_INCLUDE_DIRS}
    PARENT_SCOPE
)

# set SQAPPTOOL_LIBRARIES to PARENT_SCOPE
set(SQAPPTOOL_LIBRARIES
    ${SQAPPTOOL_LIBRARIES}
    PARENT_SCOPE
)

# --- sqtool ---
set(SOURCES
    sqtool.c
)

set(SOURCES_CPP
    sqtoolcpp.cpp
)

# ---
include_directories(${SQAPPTOOL_INCLUDE_DIRS})
link_libraries(${SQXC_LIBRARIES})

add_library(sqapp
            STATIC
            ${SOURCES_APP}
            ${HEADERS_APP}
)

add_library(sqappcpp
            STATIC
            ${SOURCES_APP_CPP}
            ${HEADERS_APP}
)

add_library(sqapptool
            STATIC
            ${SOURCES_APPTOOL}
            ${HEADERS_APPTOOL}
)

add_library(sqapptoolcpp
            STATIC
            ${SOURCES_APPTOOL_CPP}
            ${HEADERS_APPTOOL}
)


add_executable(sqtool  ${SOURCES})
target_link_libraries(sqtool  ${SQAPPTOOL_LIBRARIES})

add_executable(sqtoolcpp  ${SOURCES_CPP})
target_link_libraries(sqtoolcpp  ${SQAPPTOOL_LIBRARIES})
