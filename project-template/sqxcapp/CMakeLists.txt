# --- 'sqxcapp-user' sources and headers ---
set(SOURCES_APP_USER
    SqApp-config.c
    migrations.c
    migrations-files.c
)

if (DEFINED CMAKE_CXX_COMPILER)
	# add 'sqxcapp-user' C++ source file
	set(SOURCES_APP_USER
	    ${SOURCES_APP_USER}
	    migrations-files.cpp
	)
endif (DEFINED CMAKE_CXX_COMPILER)

set(HEADERS_APP_USER
    SqApp-config.h
    migrations.h
)

set(SQXCAPP_INCLUDE_DIRS
    ${SQXC_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SQXCAPP_LIBRARIES
    sqxcapp-user
    ${SQXC_LIBRARIES}
)

# --- 'sqxcapptool-user' sources and headers ---
set(SOURCES_APPTOOL_USER
    SqApp-config.c
    migrations.c
    SqAppTool-migrations-files.c
)

if (DEFINED CMAKE_CXX_COMPILER)
	# add 'sqxcapptool-user' C++ source file
	set(SOURCES_APPTOOL_USER
	    ${SOURCES_APPTOOL_USER}
	    SqAppTool-migrations-files.cpp
	)
endif (DEFINED CMAKE_CXX_COMPILER)

set(HEADERS_APPTOOL_USER
    SqAppTool-config.h
)

set(SQXCAPPTOOL_INCLUDE_DIRS
    ${SQXC_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SQXCAPPTOOL_LIBRARIES
    sqxcapptool-user
    ${SQXC_LIBRARIES}
)

# --- 'sqtool' sources ---
set(SOURCES
    sqtool.c
)

set(SOURCES_CPP
    sqtool-cpp.cpp
)

# --- build library ---
include_directories(${SQXCAPPTOOL_INCLUDE_DIRS})
link_libraries(${SQXC_LIBRARIES})

# 'sqxcapp-user' has database settings and migration data for user application.
add_library(sqxcapp-user
            STATIC
            ${SOURCES_APP_USER}
            ${HEADERS_APP}
)

# 'sqxcapptool-user' has database settings and migration data for user application.
add_library(sqxcapptool-user
            STATIC
            ${SOURCES_APPTOOL_USER}
            ${HEADERS_APPTOOL}
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
