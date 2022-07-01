cmake_minimum_required(VERSION 3.14)

project(sqxclib VERSION 0.6.3)

# set(CMAKE_INSTALL_PREFIX "/usr" CACHE STRING " " FORCE )

set(SQXCLIB_SOVERSION      0)

if (DEFINED CMAKE_CXX_COMPILER)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_STANDARD 11)
endif (DEFINED CMAKE_CXX_COMPILER)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/"
)

# set(CMAKE_C_FLAGS    "${CMAKE_C_FLAGS} -DHAVE_CONFIG_H")

# define CMAKE_INSTALL_<dir> and CMAKE_INSTALL_FULL_<dir>
include(GNUInstallDirs)

option(BUILD_SHARED_LIBS "Build the shared library" OFF)

# use find_package(PkgConfig REQUIRED) before use pkg-config module
find_package(PkgConfig REQUIRED)

# pkg-config module
if (PKG_CONFIG_FOUND)
	pkg_check_modules(JSONC json-c)

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

## find pthread
#if (UNIX)
#	set(THREADS_PREFER_PTHREAD_FLAG ON)
#	find_package(Threads REQUIRED)
#	set(Threads_LIBRARIES Threads::Threads)
#endif (UNIX)

# find SQLite3 if sqlite3.pc not found
if (NOT DEFINED SQLite3_FOUND OR NOT SQLite3_FOUND)
	find_package(SQLite3)
endif ()

# find MySQL if mysqlclient.pc (or mariadb.pc) not found
if (NOT DEFINED MYSQL_FOUND OR NOT MYSQL_FOUND)
	find_package(MYSQL)
endif()

# --- thread ---
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
	set(SQXCLIB_HAVE_THREAD    1)
elseif (WIN32)
	set(SQXCLIB_HAVE_THREAD    1)
else()
	set(SQXCLIB_HAVE_THREAD    0)
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
	# --- debug message ---
	message(STATUS "json-c include dirs: ${JSONC_INCLUDE_DIRS}")
	message(STATUS "json-c libraries: ${JSONC_LIBRARIES}")
	# --- config.h ---
	set(SQXCLIB_HAVE_JSONC     1)
else()
	set(SQXCLIB_HAVE_JSONC     0)
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
	# --- debug message ---
	message(STATUS "SQLite include dirs: ${SQLite3_INCLUDE_DIRS}")
	message(STATUS "SQLite libraries: ${SQLite3_LIBRARIES}")
	# --- config.h ---
	set(SQXCLIB_HAVE_SQLITE    1)
else()
	set(SQXCLIB_HAVE_SQLITE    0)
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
	# --- config.h ---
	set(SQXCLIB_HAVE_MYSQL     1)
else()
	set(SQXCLIB_HAVE_MYSQL     0)
endif (MYSQL_FOUND)

# --- pkg-config metadata file --- share/pkgconfig/sqxclib.pc
set(SQXCLIB_PC_PREFIX  ${CMAKE_INSTALL_PREFIX})
set(SQXCLIB_PC_VERSION ${PROJECT_VERSION})

if (DEFINED SQXCLIB_HAVE_JSONC)
	set(SQXCLIB_PC_REQUIRE_JSONC    "json-c")
endif ()

if (DEFINED SQXCLIB_HAVE_SQLITE)
	if (DEFINED SQLITE3_PC_FOUND)
		set(SQXCLIB_PC_REQUIRE_SQLITE   "sqlite3")
	endif()
	set(SQXCLIB_PC_LIB_SQLITE       "-lsqxc_sqlite")
endif (DEFINED SQXCLIB_HAVE_SQLITE)

if (DEFINED SQXCLIB_HAVE_MYSQL)
	# MySQL pkg-config name: mysqlclient or mariadb
	if (DEFINED MYSQLCLIENT_PC_FOUND)
		set(SQXCLIB_PC_REQUIRE_MYSQL    "mysqlclient")
	elseif (DEFINED MARIADB_PC_FOUND)
		set(SQXCLIB_PC_REQUIRE_MYSQL    "mariadb")
	endif ()
	set(SQXCLIB_PC_LIB_MYSQL        "-lsqxc_mysql")
endif (DEFINED SQXCLIB_HAVE_MYSQL)

if (DEFINED CMAKE_CXX_COMPILER)
	set(SQXCLIB_PC_LIB_CXX          "-lsqxcpp")
endif ()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    @ONLY
)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig
)

# Enable testing for the project
enable_testing()

# install project-template
install(DIRECTORY   project-template
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/sqxclib)

# --- subdirectory ---
add_subdirectory(sqxc)
add_subdirectory(sqxcsupport)
add_subdirectory(sqxcapp)
add_subdirectory(tests)
add_subdirectory(examples)

# --- debug message ---
message(STATUS "sqxclib include dirs: ${SQXC_INCLUDE_DIRS}")
message(STATUS "sqxclib libraries: ${SQXC_LIBRARIES}")
