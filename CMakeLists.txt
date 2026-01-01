cmake_minimum_required(VERSION 3.14)

project(sqxclib
    VERSION 1.1
    HOMEPAGE_URL "https://github.com/chhuang-one/sqxclib/"
#   HOMEPAGE_URL "https://gitee.com/chhuang-one/sqxclib/"
)

set(SQXCLIB_SOVERSION      0)

# Don't specify version of the C or C++ language standard. (Except for macOS.)
# Note: sqxclib code still compile with C99 and C++11 currently.
if (DEFINED CMAKE_CXX_COMPILER)
	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		# AppleClang use old C++ standard by default
		set(CMAKE_CXX_STANDARD_REQUIRED ON)
		set(CMAKE_CXX_STANDARD 11)
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		# MSVC++ must support parameter pack in constructor and standards preprocessor.
#		set(MSVC_COMPILER_FLAGS  "/experimental:preprocessor")
		set(MSVC_COMPILER_FLAGS  "/Zc:preprocessor")
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${MSVC_COMPILER_FLAGS}")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_COMPILER_FLAGS}")
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		# GCC has link flags to avoid linking unused shared libraries
		set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,--as-needed")
#		set(CMAKE_CXX_STANDARD_REQUIRED ON)
#		set(CMAKE_CXX_STANDARD 11)
	else ()
		# Other C++ compilers
#		set(CMAKE_CXX_STANDARD_REQUIRED ON)
#		set(CMAKE_CXX_STANDARD 11)
	endif ()
endif (DEFINED CMAKE_CXX_COMPILER)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/"
)

# set(CMAKE_C_FLAGS    "${CMAKE_C_FLAGS} -DHAVE_CONFIG_H")
# set(CMAKE_INSTALL_PREFIX "/usr" CACHE STRING " " FORCE )

# define CMAKE_INSTALL_<dir> and CMAKE_INSTALL_FULL_<dir>
include(GNUInstallDirs)

# options
option(BUILD_SHARED_LIBS "Build the shared library" OFF)

# pkg-config is not required, as not every platform has it.
# find_package(PkgConfig REQUIRED)
find_package(PkgConfig)

# pkg-config module
if (PKG_CONFIG_FOUND)
	pkg_check_modules(cJSON libcjson)
	if (cJSON_FOUND)
		set(cJSON_PC_FOUND true)
		set(CJSON_INCLUDE_DIRS ${cJSON_INCLUDE_DIRS})
		set(CJSON_LIBRARIES    ${cJSON_LIBRARIES})
	endif (cJSON_FOUND)

	pkg_check_modules(json-c json-c)
	if (json-c_FOUND)
		set(json-c_PC_FOUND true)
	endif (json-c_FOUND)

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
		endif ()
	endif (MYSQL_FOUND)

	pkg_check_modules(PostgreSQL libpq)
	if (PostgreSQL_FOUND)
		# for Windows: If no directory in PostgreSQL_INCLUDE_DIRS
		if (CMAKE_C_COMPILER_ID STREQUAL "MSVC" AND PostgreSQL_INCLUDE_DIRS STREQUAL "")
			set(PostgreSQL_FOUND false)
		else ()
			set(PostgreSQL_PC_FOUND true)
		endif ()
	endif (PostgreSQL_FOUND)
endif (PKG_CONFIG_FOUND)

# find cJSON if libcjson.pc not found
if (NOT cJSON_FOUND)
	find_package(cJSON)
endif ()

# find json-c if json-c.pc not found
if (NOT json-c_FOUND)
	find_package(json-c)
endif ()

# find SQLite3 if sqlite3.pc not found
if (NOT SQLite3_FOUND)
	find_package(SQLite3)
endif ()

# find MySQL if mysqlclient.pc (or mariadb.pc) not found
if (NOT MYSQL_FOUND)
	find_package(MYSQL)
endif ()

# find PostgreSQL if libpq.pc not found
if (NOT PostgreSQL_FOUND)
	find_package(PostgreSQL)
endif ()

# --- Sqxc include dirs ---
set(Sqxc_INCLUDE_DIRS  ${CMAKE_CURRENT_SOURCE_DIR})

# --- cJSON ---
if (cJSON_FOUND)
	set(Sqxc_INCLUDE_DIRS
	    ${Sqxc_INCLUDE_DIRS}
	    ${CJSON_INCLUDE_DIRS}
	)
	set(Sqxc_LIBRARIES
	    ${Sqxc_LIBRARIES}
	    ${CJSON_LIBRARIES}
	)
	# --- debug message ---
	message(STATUS "cJSON include dirs: ${CJSON_INCLUDE_DIRS}")
	message(STATUS "cJSON libraries: ${CJSON_LIBRARIES}")
	# --- config.h ---
	set(SQXCLIB_HAVE_CJSON     1)
else ()
	set(SQXCLIB_HAVE_CJSON     0)
endif (cJSON_FOUND)

# --- json-c ---
if (json-c_FOUND)
	set(json-c_LIBRARIES json-c)
	set(Sqxc_INCLUDE_DIRS
	    ${Sqxc_INCLUDE_DIRS}
	    ${json-c_INCLUDE_DIRS}
	)
	set(Sqxc_LIBRARIES
	    ${Sqxc_LIBRARIES}
	    ${json-c_LIBRARIES}
	)
	# --- debug message ---
	message(STATUS "json-c include dirs: ${json-c_INCLUDE_DIRS}")
	message(STATUS "json-c libraries: ${json-c_LIBRARIES}")
	# --- config.h ---
	set(SQXCLIB_HAVE_JSONC     1)
else ()
	set(SQXCLIB_HAVE_JSONC     0)
endif (json-c_FOUND)

# --- SQLite ---
if (SQLite3_FOUND)
	# ${SQLite3_INCLUDE_DIRS} and ${SQLite3_LIBRARIES} will be used in sqxc/CMakeLists.txt
	# --- debug message ---
	message(STATUS "SQLite include dirs: ${SQLite3_INCLUDE_DIRS}")
	message(STATUS "SQLite libraries: ${SQLite3_LIBRARIES}")
	# --- config.h ---
	set(SQXCLIB_HAVE_SQLITE    1)
else ()
	set(SQXCLIB_HAVE_SQLITE    0)
endif (SQLite3_FOUND)

# --- MySQL ---
if (MYSQL_FOUND)
	# ${MYSQL_INCLUDE_DIRS} and ${MYSQL_LIBRARIES} will be used in sqxc/CMakeLists.txt
	# --- debug message ---
	message(STATUS "MySQL include dirs: ${MYSQL_INCLUDE_DIRS}")
	message(STATUS "MySQL libraries: ${MYSQL_LIBRARIES}")
	# --- config.h ---
	set(SQXCLIB_HAVE_MYSQL     1)
else ()
	set(SQXCLIB_HAVE_MYSQL     0)
endif (MYSQL_FOUND)

# --- PostgreSQL ---
if (PostgreSQL_FOUND)
	# ${PostgreSQL_INCLUDE_DIRS} and ${PostgreSQL_LIBRARIES} will be used in sqxc/CMakeLists.txt
	# --- debug message ---
	message(STATUS "PostgreSQL include dirs: ${PostgreSQL_INCLUDE_DIRS}")
	message(STATUS "PostgreSQL libraries: ${PostgreSQL_LIBRARIES}")
	# --- config.h ---
	set(SQXCLIB_HAVE_POSTGRESQL     1)
else ()
	set(SQXCLIB_HAVE_POSTGRESQL     0)
endif (PostgreSQL_FOUND)

# --- pkg-config metadata file --- share/pkgconfig/sqxclib.pc
set(SQXCLIB_PC_PREFIX  ${CMAKE_INSTALL_PREFIX})
set(SQXCLIB_PC_VERSION ${PROJECT_VERSION})

if (SQXCLIB_HAVE_CJSON)
	if (DEFINED cJSON_PC_FOUND AND cJSON_PC_FOUND)
		set(SQXCLIB_PC_REQUIRE_CJSON    "libcjson")
	endif ()
endif ()

if (SQXCLIB_HAVE_JSONC)
	if (DEFINED json-c_PC_FOUND AND json-c_PC_FOUND)
		set(SQXCLIB_PC_REQUIRE_JSONC    "json-c")
	endif ()
endif ()

if (SQXCLIB_HAVE_SQLITE)
	if (DEFINED SQLite3_PC_FOUND AND SQLite3_PC_FOUND)
		set(SQXCLIB_PC_REQUIRE_SQLITE   "sqlite3")
	endif ()
	set(SQXCLIB_PC_LIB_SQLITE       "-lsqxc_sqlite")
endif ()

if (SQXCLIB_HAVE_MYSQL)
	# MySQL pkg-config name: mysqlclient or mariadb
	if (DEFINED MYSQLCLIENT_PC_FOUND AND MYSQLCLIENT_PC_FOUND)
		set(SQXCLIB_PC_REQUIRE_MYSQL    "mysqlclient")
	elseif (DEFINED MARIADB_PC_FOUND AND MARIADB_PC_FOUND)
		set(SQXCLIB_PC_REQUIRE_MYSQL    "mariadb")
	endif ()
	set(SQXCLIB_PC_LIB_MYSQL        "-lsqxc_mysql")
endif ()

if (SQXCLIB_HAVE_POSTGRESQL)
	if (DEFINED PostgreSQL_PC_FOUND AND PostgreSQL_PC_FOUND)
		set(SQXCLIB_PC_REQUIRE_POSTGRESQL  "libpq")
	endif ()
	set(SQXCLIB_PC_LIB_POSTGRESQL       "-lsqxc_postgre")
endif ()

if (DEFINED CMAKE_CXX_COMPILER)
	set(SQXCLIB_PC_LIB_CXX          "-lsqxcxx")
endif ()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    @ONLY
)
# install pkg-config metadata file
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig
)
# --- End of pkg-config metadata file ---

# install CMake module
set(MODULE_INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Modules")
install(FILES "cmake/FindSqxc.cmake"
        DESTINATION "${MODULE_INSTALL_DESTINATION}")

# install project-template
install(DIRECTORY   "project-template"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/sqxclib)

# Enable testing for the project
enable_testing()

# --- subdirectory ---
add_subdirectory(sqxc)
add_subdirectory(sqxc/support)
add_subdirectory(sqxc/app)
add_subdirectory(sqapp)
add_subdirectory(tests)
add_subdirectory(examples)

# --- debug message ---
message(STATUS "Sqxc (sqxclib) include dirs: ${Sqxc_INCLUDE_DIRS}")
message(STATUS "Sqxc (sqxclib) libraries: ${Sqxc_LIBRARIES}")
