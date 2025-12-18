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

set(SqxcApp_INCLUDE_DIRS
    ${Sqxc_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SqxcApp_LIBRARIES
    sqxcapp-data
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
    sqxcapp-data
    sqxcapptool
    sqxcapp
    ${SqxcSupport_LIBRARIES}
)

# --- 'sqtool' sources ---
set(SOURCES
    sqtool.c
)

set(SOURCES_CPP
    sqtool-cpp.cpp
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
target_link_libraries(sqtool  ${SqxcAppTool_LIBRARIES})

# --- 'sqtool-cpp' executable file ---
if (DEFINED CMAKE_CXX_COMPILER)
	add_executable(sqtool-cpp  ${SOURCES_CPP})
	target_link_libraries(sqtool-cpp  ${SqxcAppTool_LIBRARIES})
endif (DEFINED CMAKE_CXX_COMPILER)

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
