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

set(SOURCES_APP_DATA
    SqApp-config.c
    migrations.c
    migrations-files.c
)

if (DEFINED CMAKE_CXX_COMPILER)
	# add 'sqxcapp' C++ source file
	set(SOURCES_APP_DATA
	    ${SOURCES_APP_DATA}
	    migrations-files-cxx.cpp
	)
endif (DEFINED CMAKE_CXX_COMPILER)

set(HEADERS_APP_DATA
    SqApp-config.h
    migrations.h
)

set(SQXCAPP_INCLUDE_DIRS
    ${SQXC_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SQXCAPP_LIBRARIES
    sqxcapp-data
    sqxcapp
    ${SQXC_LIBRARIES}
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

set(SQXCAPPTOOL_INCLUDE_DIRS
    ${SQXCSUPPORT_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SQXCAPPTOOL_LIBRARIES
    sqxcapp-data
    sqxcapptool
    sqxcapp
    ${SQXCSUPPORT_LIBRARIES}
)

# --- 'sqtool' sources ---
set(SOURCES
    sqtool.c
)

set(SOURCES_CPP
    sqtool-cpp.cpp
)

# --- build ---
include_directories(${SQXCAPPTOOL_INCLUDE_DIRS})

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
#	target_link_libraries(sqxcapp ${SQXC_LIBRARIES})
endif (BUILD_SHARED_LIBS)

# 'sqxcapp-data' has database settings and migration data for user application.
add_library(sqxcapp-data
            STATIC
            ${SOURCES_APP_DATA}
            ${HEADERS_APP}
)

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

# --- 'sqtool' executable file ---
add_executable(sqtool  ${SOURCES})
target_link_libraries(sqtool  ${SQXCAPPTOOL_LIBRARIES})

# --- 'sqtool-cpp' executable file ---
if (DEFINED CMAKE_CXX_COMPILER)
	add_executable(sqtool-cpp  ${SOURCES_CPP})
	target_link_libraries(sqtool-cpp  ${SQXCAPPTOOL_LIBRARIES})
endif (DEFINED CMAKE_CXX_COMPILER)

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
