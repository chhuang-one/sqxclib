cmake_minimum_required(VERSION 3.14)

project(user-project VERSION 1.0)

if (DEFINED CMAKE_CXX_COMPILER)
	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		# AppleClang use old C++ standard by default
		set(CMAKE_CXX_STANDARD_REQUIRED ON)
		set(CMAKE_CXX_STANDARD 11)
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		# MSVC++ must support parameter pack in constructor
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		# GCC has link flags to avoid linking unused shared libraries
		set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,--as-needed")
	endif ()
endif (DEFINED CMAKE_CXX_COMPILER)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/"
)

# set(CMAKE_C_FLAGS    "${CMAKE_C_FLAGS} -DHAVE_CONFIG_H")

# use find_package(PkgConfig REQUIRED) before use pkg-config module
find_package(PkgConfig REQUIRED)

# pkg-config module
if (PKG_CONFIG_FOUND)
	pkg_check_modules(SQXC sqxclib)

	pkg_check_modules(SQLite3 sqlite3)
	if (SQLite3_FOUND)
		set(SQLite3_PC_FOUND true)
	endif (SQLite3_FOUND)

	pkg_check_modules(MYSQL mysqlclient)
	if (MYSQL_FOUND)
		set(MYSQLCLIENT_PC_FOUND true)
	else ()
		pkg_check_modules(MYSQL mariadb)
		if (MYSQL_FOUND)
			set(MARIADB_PC_FOUND true)
		endif()
	endif(MYSQL_FOUND)

	pkg_check_modules(PostgreSQL libpq)
	if (PostgreSQL_FOUND)
		set(PostgreSQL_PC_FOUND true)
	endif (PostgreSQL_FOUND)
endif (PKG_CONFIG_FOUND)

# find SQLite3 if sqlite3.pc not found
if (NOT DEFINED SQLite3_FOUND OR NOT SQLite3_FOUND)
	find_package(SQLite3)
endif ()

# find MySQL if mysqlclient.pc (or mariadb.pc) not found
if (NOT DEFINED MYSQL_FOUND OR NOT MYSQL_FOUND)
	find_package(MYSQL)
endif()

# find PostgreSQL if libpq.pc not found
if (NOT DEFINED PostgreSQL_FOUND OR NOT PostgreSQL_FOUND)
	find_package(PostgreSQL)
endif ()

# --- SQLite ---
if (SQLite3_FOUND AND NOT DEFINED SQLite3_PC_FOUND)
	set(SQXC_INCLUDE_DIRS
	    ${SQXC_INCLUDE_DIRS}
	    ${SQLite3_INCLUDE_DIRS}
	)
	set(SQXC_LIBRARIES
	    ${SQXC_LIBRARIES}
	    ${SQLite3_LIBRARIES}
	)
	# --- debug message ---
	message(STATUS "SQLite include dirs: ${SQLite3_INCLUDE_DIRS}")
	message(STATUS "SQLite libraries: ${SQLite3_LIBRARIES}")
endif (SQLite3_FOUND AND NOT DEFINED SQLite3_PC_FOUND)

# --- MySQL ---
if (MYSQL_FOUND AND (NOT DEFINED MYSQLCLIENT_PC_FOUND AND NOT DEFINED MARIADB_PC_FOUND))
	set(SQXC_INCLUDE_DIRS
	    ${SQXC_INCLUDE_DIRS}
	    ${MYSQL_INCLUDE_DIRS}
	)
	set(SQXC_LIBRARIES
	    ${SQXC_LIBRARIES}
	    ${MYSQL_LIBRARIES}
	)
	# --- Visual Studio 2015 and later ---
	if (MSVC AND NOT (MSVC_VERSION LESS 1900))
		set(SQXC_LIBRARIES
		    ${SQXC_LIBRARIES}
		    legacy_stdio_definitions.lib
		)
	endif()
	# --- debug message ---
	message(STATUS "MySQL include dirs: ${MYSQL_INCLUDE_DIRS}")
	message(STATUS "MySQL libraries: ${MYSQL_LIBRARIES}")
endif (MYSQL_FOUND AND (NOT DEFINED MYSQLCLIENT_PC_FOUND AND NOT DEFINED MARIADB_PC_FOUND))

# --- PostgreSQL ---
if (PostgreSQL_FOUND AND NOT DEFINED PostgreSQL_PC_FOUND)
	set(SQXC_INCLUDE_DIRS
	    ${SQXC_INCLUDE_DIRS}
	    ${PostgreSQL_INCLUDE_DIRS}
	)
	set(SQXC_LIBRARIES
	    ${SQXC_LIBRARIES}
	    ${PostgreSQL_LIBRARIES}
	)
	# --- debug message ---
	message(STATUS "PostgreSQL include dirs: ${PostgreSQL_INCLUDE_DIRS}")
	message(STATUS "PostgreSQL libraries: ${PostgreSQL_LIBRARIES}")
endif (PostgreSQL_FOUND AND NOT DEFINED PostgreSQL_PC_FOUND)

# --- subdirectory ---
add_subdirectory(sqxcapp)
add_subdirectory(src)

# --- debug message ---
message(STATUS "sqxclib include dirs: ${SQXC_INCLUDE_DIRS}")
message(STATUS "sqxclib libraries: ${SQXC_LIBRARIES}")
