# --- sqapp ---
set(SOURCES_APP
    SqApp.c
    migrations.c
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
    ${SQXC_LIBRARIES}
)

# --- sqapptool ---
set(SOURCES_TOOL
    SqApp.c
    SqAppTool.c
    SqAppTool-migrations.c
    CommonOptions.c
    CommandMigrate.c
)

set(HEADERS_TOOL
    SqApp.h
    SqApp-config.h
    SqAppTool.h
    migrations.h
    CommonOptions.h
    CommandMigrate.h
)

set(SQTOOL_INCLUDE_DIRS
    ${SQAPP_INCLUDE_DIRS}
)

set(SQTOOL_LIBRARIES
    sqapptool
    sqxconsole
    ${SQXC_LIBRARIES}
)

# --- sqtool ---
set(SOURCES
    sqtool.c
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

add_library(sqapptool
            STATIC
            ${SOURCES_TOOL}
            ${HEADERS_TOOL}
)


add_executable(sqtool
               ${SOURCES}
)
target_link_libraries(sqtool
                      ${SQTOOL_LIBRARIES}
)
