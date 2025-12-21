# --- 'sqxcapp' sources and headers ---
set(SOURCES_APP
    SqApp.c
    SqMigration.c
)

set(HEADERS_APP
    sqxcapp.h
    SqApp.h
    SqMigration.h
)

set(SqxcApp_INCLUDE_DIRS
    ${Sqxc_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SqxcApp_LIBRARIES
    sqxcapp
    ${Sqxc_LIBRARIES}
)

# --- 'sqxcapptool' sources and headers ---
set(SOURCES_APPTOOL
    SqAppTool.c
    SqAppOptions.c
    SqAppCommands-migrate.c
    SqAppCommands-make.c
)

set(HEADERS_APPTOOL
    SqAppTool.h
    SqAppOptions.h
    SqAppCommands.h
)

set(SqxcAppTool_INCLUDE_DIRS
    ${SqxcSupport_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SqxcAppTool_LIBRARIES
    sqxcapptool
    sqxcapp
    ${SqxcSupport_LIBRARIES}
)

# --- build ---
include_directories(${SqxcAppTool_INCLUDE_DIRS})

# --- 'sqxcapp' library ---
add_library(sqxcapp
            ${SOURCES_APP}
            ${HEADERS_APP}
)
set_target_properties(sqxcapp
    PROPERTIES
    VERSION        ${PROJECT_VERSION}
#    SOVERSION      ${SQXCLIB_SOVERSION}
    PUBLIC_HEADER "${HEADERS_APP}"
)
if (BUILD_SHARED_LIBS)
	target_link_libraries(sqxcapp sqxc)
#else ()
#	target_link_libraries(sqxcapp ${Sqxc_LIBRARIES})
endif (BUILD_SHARED_LIBS)

# --- 'sqxcapptool' library ---
add_library(sqxcapptool
            ${SOURCES_APPTOOL}
            ${HEADERS_APPTOOL}
)
set_target_properties(sqxcapptool
    PROPERTIES 
    VERSION        ${PROJECT_VERSION}
#    SOVERSION      ${SQXCLIB_SOVERSION}
    PUBLIC_HEADER "${HEADERS_APPTOOL}"
)
if (BUILD_SHARED_LIBS)
	target_link_libraries(sqxcapptool sqxcapp sqxcsupport)
endif(BUILD_SHARED_LIBS)

# install
install(
    TARGETS sqxcapp sqxcapptool
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sqxc/app
)

# set SqxcApp_INCLUDE_DIRS to PARENT_SCOPE
set(SqxcApp_INCLUDE_DIRS
    ${SqxcApp_INCLUDE_DIRS}
    PARENT_SCOPE
)

# set SqxcApp_LIBRARIES to PARENT_SCOPE
set(SqxcApp_LIBRARIES
    ${SqxcApp_LIBRARIES}
    PARENT_SCOPE
)

# set SqxcAppTool_INCLUDE_DIRS to PARENT_SCOPE
set(SqxcAppTool_INCLUDE_DIRS
    ${SqxcAppTool_INCLUDE_DIRS}
    PARENT_SCOPE
)

# set SqxcAppTool_LIBRARIES to PARENT_SCOPE
set(SqxcAppTool_LIBRARIES
    ${SqxcAppTool_LIBRARIES}
    PARENT_SCOPE
)
