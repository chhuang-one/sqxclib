cmake_minimum_required(VERSION 3.14)

project(user-project VERSION 1.0)

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
		set(SQLITE3_PC_FOUND true)
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
endif (PKG_CONFIG_FOUND)

# find SQLite3 if sqlite3.pc not found
if (NOT DEFINED SQLite3_FOUND OR NOT SQLite3_FOUND)
	find_package(SQLite3)
endif ()

# find MySQL if mysqlclient.pc (or mariadb.pc) not found
if (NOT DEFINED MYSQL_FOUND OR NOT MYSQL_FOUND)
	find_package(MYSQL)
endif()

# --- SQLite ---
if (SQLite3_FOUND AND NOT DEFINED SQLITE3_PC_FOUND)
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
endif (SQLite3_FOUND AND NOT DEFINED SQLITE3_PC_FOUND)

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

# --- subdirectory ---
add_subdirectory(sqxcapp)
add_subdirectory(src)

# --- debug message ---
message(STATUS "sqxclib include dirs: ${SQXC_INCLUDE_DIRS}")
message(STATUS "sqxclib libraries: ${SQXC_LIBRARIES}")
