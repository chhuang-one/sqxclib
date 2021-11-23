# --- sqapp ---
set(SOURCES_APP
    SqApp.c
    migrations.c
    migrations-files.c
)

set(SOURCES_APP_CPP
    migrations-files.cpp
)

set(HEADERS_APP
    SqApp.h
    SqApp-config.h
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

# --- sqapptool ---
set(SOURCES_TOOL
    SqApp.c
    SqPairs.c
    SqAppTool.c
    SqAppTool-migrations.c
    migrations.c
    CommonOptions.c
    CommandMigrate.c
    CommandMake.c
)

set(SOURCES_TOOL_CPP
    SqAppTool-migrations.cpp
)

set(HEADERS_TOOL
    SqApp.h
    SqApp-config.h
    SqPairs.h
    SqAppTool.h
    migrations.h
    CommonOptions.h
    CommandMigrate.h
    CommandMake.h
)

set(SQTOOL_INCLUDE_DIRS
    ${SQAPP_INCLUDE_DIRS}
)

set(SQTOOL_LIBRARIES
    sqapptool
    sqapptoolcpp
    sqxconsole
    ${SQXC_LIBRARIES}
)

# --- sqtool ---
set(SOURCES
    sqtool.c
)

set(SOURCES_CPP
    sqtoolcpp.cpp
)

# ---
include_directories(${SQAPP_INCLUDE_DIRS})
link_libraries(sqxconsole
               ${SQXC_LIBRARIES}
)

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
            ${SOURCES_TOOL}
            ${HEADERS_TOOL}
)

add_library(sqapptoolcpp
            STATIC
            ${SOURCES_TOOL_CPP}
            ${HEADERS_TOOL}
)


add_executable(sqtool
               ${SOURCES}
)
target_link_libraries(sqtool
                      ${SQTOOL_LIBRARIES}
)

add_executable(sqtoolcpp
               ${SOURCES_CPP}
)
target_link_libraries(sqtoolcpp
                      ${SQTOOL_LIBRARIES}
)
