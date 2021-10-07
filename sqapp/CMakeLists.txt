# --- sqapp ---
set(SOURCES_APP
    SqApp.c
    migrations.c
)

set(HEADERS_APP
    SqApp.h
    migrations.h
)

# --- sqapptool ---
set(SOURCES_TOOL
    SqApp.c
    SqAppTool.c
    SqAppTool-migrations.c
    CommandMigrate.c
)

set(HEADERS_TOOL
    SqApp.h
    SqAppTool.h
    migrations.h
    CommandMigrate.h
)

# --- sqtool ---
set(SOURCES
    sqtool.c
)

# ---
include_directories(${SQXC_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR}
)
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
                      sqapptool
                      sqxconsole
                      ${SQXC_LIBRARIES}
)
