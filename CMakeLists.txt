cmake_minimum_required(VERSION 3.14)

project(sqxclib)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/"
)

set(CMAKE_C_FLAGS    "${CMAKE_C_FLAGS} -DDEBUG")
# set(CMAKE_C_FLAGS    "${CMAKE_C_FLAGS} -DHAVE_CONFIG_H")

# find SQLite3
find_package(SQLite3)

# find MySQL
find_package(MYSQL)

# use find_package(PkgConfig REQUIRED) before use pkg-config module
find_package(PkgConfig REQUIRED)

# pkg-config module
if (PKG_CONFIG_FOUND)
	pkg_check_modules(JSONC json-c)
	if (NOT SQLite3_FOUND)
		pkg_check_modules(SQLite3 sqlite3)
	endif()
endif()

# --- json-c ---
if (JSONC_FOUND)
	set(SQXC_INCLUDE_DIRS
	    ${SQXC_INCLUDE_DIRS}
	    ${JSONC_INCLUDE_DIRS}
	)
	set(SQXC_LIBRARIES
	    ${SQXC_LIBRARIES}
	    ${JSONC_LIBRARIES}
	)
	# --- config.h ---
	set(have_jsonc     1)
else()
	set(have_jsonc     0)
endif()

# --- SQLite ---
if (SQLite3_FOUND)
	set(SQXC_INCLUDE_DIRS
	    ${SQXC_INCLUDE_DIRS}
	    ${SQLite3_INCLUDE_DIRS}
	)
	set(SQXC_LIBRARIES
	    ${SQXC_LIBRARIES}
	    ${SQLite3_LIBRARIES}
	)
	# --- config.h ---
	set(have_sqlite    1)
else()
	set(have_sqlite    0)
endif()

# --- MySQL ---
if (MYSQL_FOUND)
	set(SQXC_INCLUDE_DIRS
	    ${SQXC_INCLUDE_DIRS}
	    ${MYSQL_INCLUDE_DIRS}
	)
	set(SQXC_LIBRARIES
	    ${SQXC_LIBRARIES}
	    ${MYSQL_LIBRARIES}
	)
	message(STATUS "MySQL include dirs: ${MYSQL_INCLUDE_DIRS}")
	# --- config.h ---
	set(have_mysql     1)
else()
	set(have_mysql     0)
endif()

# --- config.h ---
configure_file("${CMAKE_SOURCE_DIR}/sqxc/config.h.in"
               "${CMAKE_SOURCE_DIR}/sqxc/config.h")

# --- sqxc ---
set(SQXC_INCLUDE_DIRS
    ${SQXC_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/sqxc"
)
add_subdirectory(sqxc)
set(SQXC_LIBRARIES
    ${SQXC_LIBRARIES}
    sqxc
    sqxcpp
)

# --- others ---
add_subdirectory(examples)
