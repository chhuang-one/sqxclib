cmake_minimum_required(VERSION 3.14)

project(sqxclib VERSION 0.6.2)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/"
)

# set(CMAKE_C_FLAGS    "${CMAKE_C_FLAGS} -DHAVE_CONFIG_H")

# find pthread
if (UNIX)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	set(Threads_LIBRARIES Threads::Threads)
endif (UNIX)

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
endif (PKG_CONFIG_FOUND)

# --- thrread ---
if (Threads_FOUND)
	set(SQXC_INCLUDE_DIRS
	    ${SQXC_INCLUDE_DIRS}
	    ${Threads_INCLUDE_DIRS}
	)
	set(SQXC_LIBRARIES
	    ${SQXC_LIBRARIES}
	    ${Threads_LIBRARIES}
	)
# --- config.h ---
	set(have_thread    1)
elseif (WIN32)
	set(have_thread    1)
else()
	set(have_thread    0)
endif (Threads_FOUND)

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
endif (JSONC_FOUND)

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
endif (SQLite3_FOUND)

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
	message(STATUS "MySQL libraries: ${MYSQL_LIBRARIES}")
	# --- config.h ---
	set(have_mysql     1)
else()
	set(have_mysql     0)
endif (MYSQL_FOUND)

# --- config.h ---
configure_file("${CMAKE_SOURCE_DIR}/sqxc/config.h.in"
               "${CMAKE_SOURCE_DIR}/sqxc/config.h")

# Enable testing for the project
enable_testing()

# --- subdirectory ---
add_subdirectory(sqxc)
add_subdirectory(sqxcsupport)
add_subdirectory(sqxcapp)
add_subdirectory(tests)
add_subdirectory(examples)
