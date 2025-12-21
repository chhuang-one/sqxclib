# --- 'sqxcapp-data' sources and headers ---
set(SOURCES_APP_DATA
    SqApp-config.c
    migrations.c
    migrations-files.c
)

if (DEFINED CMAKE_CXX_COMPILER)
	# add 'sqxcapp-data' C++ source file
	set(SOURCES_APP_DATA
	    ${SOURCES_APP_DATA}
	    migrations-files-cxx.cpp
	)
endif (DEFINED CMAKE_CXX_COMPILER)

set(HEADERS_APP_DATA
    SqApp-config.h
    migrations.h
)

# --- 'sqtool' sources ---
set(SOURCES
    sqtool.c
)

set(SOURCES_CPP
    sqtool-cpp.cpp
)

# --- set 'sqxcapp' include dirs and libraries ---
set(SqxcApp_INCLUDE_DIRS
    ${Sqxc_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SqxcApp_LIBRARIES
    sqxcapp-data
    ${Sqxc_LIBRARIES}
)

# --- set 'sqxcapptool' include dirs and libraries ---
set(SqxcAppTool_INCLUDE_DIRS
    ${Sqxc_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(SqxcAppTool_LIBRARIES
    sqxcapp-data
    ${Sqxc_LIBRARIES}
)

# --- build 'sqxcapp-data' & 'sqtool' ---
include_directories(${SqxcAppTool_INCLUDE_DIRS})

# --- 'sqxcapp-data' static library ---
# It has database settings and migration data for user application.
add_library(sqxcapp-data
            STATIC
            ${SOURCES_APP_DATA}
            ${HEADERS_APP}
)

# --- 'sqtool' executable file ---
add_executable(sqtool  ${SOURCES})
target_link_libraries(sqtool  ${SqxcAppTool_LIBRARIES})

# --- 'sqtool-cpp' executable file ---
if (DEFINED CMAKE_CXX_COMPILER)
	add_executable(sqtool-cpp  ${SOURCES_CPP})
	target_link_libraries(sqtool-cpp  ${SqxcAppTool_LIBRARIES})
endif (DEFINED CMAKE_CXX_COMPILER)

# -----------------------------------------------------------------------------

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
