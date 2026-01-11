# helpers sources & headers
set(SOURCES_HELPERS
    SqHelpers.c
)

set(HEADERS_HELPERS
    SqHelpers.h
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
    SqxcMem.c
)

set(HEADERS_TEST
    SqdbEmpty.h
    SqxcEmpty.h
    SqxcFile.h
    SqxcMem.h
)

# all headers
set(HEADERS
    ${HEADERS_HELPERS}
    ${HEADERS_STRUCTS}
    ${HEADERS_CONSOLE}
    ${HEADERS_TEST}
    sqxcsupport.h
)

# set SqxcSupport_INCLUDE_DIRS to PARENT_SCOPE
set(SqxcSupport_INCLUDE_DIRS
    ${Sqxc_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    PARENT_SCOPE
)

# set SqxcSupport_LIBRARIES to PARENT_SCOPE
set(SqxcSupport_LIBRARIES
    sqxcsupport
    ${Sqxc_LIBRARIES}
    PARENT_SCOPE
)

# --- build ---
include_directories(${Sqxc_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}")

# --- sqxcsupport library ---
add_library(sqxcsupport
            ${SOURCES_HELPERS}
            ${SOURCES_STRUCTS}
            ${SOURCES_CONSOLE}
            ${SOURCES_TEST}

            ${HEADERS_HELPERS}
            ${HEADERS_STRUCTS}
            ${HEADERS_CONSOLE}
            ${HEADERS_TEST}
)
set_target_properties(sqxcsupport
    PROPERTIES
    VERSION        ${PROJECT_VERSION}
#    SOVERSION      ${SQXCLIB_SOVERSION}
    PUBLIC_HEADER "${HEADERS}"
)
if (BUILD_SHARED_LIBS)
    target_link_libraries(sqxcsupport sqxc)
#else ()
#	target_link_libraries(sqxcsupport ${Sqxc_LIBRARIES})
endif (BUILD_SHARED_LIBS)

# install
install(
    TARGETS sqxcsupport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sqxc/support
)
