#
#   Copyright (C) 2025 by C.H. Huang
#   plushuang.tw@gmail.com
#
# sqxclib is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.
#

# - Try to find Sqxc (sqxclib)
# Sqxc Project site: https://github.com/chhuang-one/sqxclib/
#                    https://gitee.com/chhuang-one/sqxclib/

# You can also use pkg_check_modules(Sqxc sqxclib) to find dependency packages.

# Sqxc_FOUND        - If false, do not try to use sqxclib.
# Sqxc_INCLUDE_DIRS - Where to find sqxclib.h, etc.
# Sqxc_LIBRARIES    - The libraries to link against.

find_path(Sqxc_INCLUDE_DIR
    NAMES "sqxc/sqxclib.h"
    PATHS "/usr/include"
          "/usr/local/include"
)

find_library(Sqxc_LIBRARY
    NAMES "sqxc"
    PATHS "/lib"
          "/lib64"
          "/usr/lib"
          "/usr/lib64"
          "/usr/local/lib"
          "/usr/local/lib64"
)

# handle the QUIETLY and REQUIRED arguments and set Sqxc_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Sqxc
    REQUIRED_VARS    Sqxc_LIBRARY Sqxc_INCLUDE_DIR
)

if (Sqxc_FOUND)
	set(Sqxc_INCLUDE_DIRS
	    ${Sqxc_INCLUDE_DIR}
	    ${Sqxc_INCLUDE_DIR}/sqxc
	    ${Sqxc_INCLUDE_DIR}/sqxc/app
	    ${Sqxc_INCLUDE_DIR}/sqxc/support
	)

	set(Sqxc_LIBRARIES
	    sqxc
	)

	# sqxclib contains several libraries to support different SQL products.
	# find other sqxclib libraries in directory.
	set(LibList
#	    sqxc
	    sqxcpp
	    sqxc_sqlite
	    sqxc_mysql
	    sqxc_postgre
	    sqxcapp
	    sqxcsupport
	    sqxcapptool
	)

	foreach(CurLib ${LibList})
		find_library(CurLibPath
		    NAMES "${CurLib}"
		    PATHS "/lib"
		          "/lib64"
		          "/usr/lib"
		          "/usr/lib64"
		          "/usr/local/lib"
		          "/usr/local/lib64"
		)

		if (CurLibPath)
			set(Sqxc_LIBRARIES
			    ${CurLib}
			    ${Sqxc_LIBRARIES}
			)
			# find dependency SQL packages
			if (CurLib STREQUAL "sqxc_sqlite")
				find_package(SQLite3)
			elseif (CurLib STREQUAL "sqxc_mysql")
				find_package(MYSQL)
			elseif (CurLib STREQUAL "sqxc_postgre")
				find_package(PostgreSQL)
			endif ()
		endif ()

		unset(CurLibPath CACHE)
		unset(CurLibPath)
	endforeach()

	# set Sqxc_INCLUDE_DIRS and Sqxc_LIBRARIES
	# --- SQLite ---
	if (SQLite3_FOUND)
		set(Sqxc_INCLUDE_DIRS
		    ${Sqxc_INCLUDE_DIRS}
		    ${SQLite3_INCLUDE_DIRS}
		)
		set(Sqxc_LIBRARIES
		    ${Sqxc_LIBRARIES}
		    ${SQLite3_LIBRARIES}
		)
		# --- debug message ---
		message(STATUS "SQLite include dirs: ${SQLite3_INCLUDE_DIRS}")
		message(STATUS "SQLite libraries: ${SQLite3_LIBRARIES}")
	endif (SQLite3_FOUND)

	# --- MySQL ---
	if (MYSQL_FOUND)
		set(Sqxc_INCLUDE_DIRS
		    ${Sqxc_INCLUDE_DIRS}
		    ${MYSQL_INCLUDE_DIRS}
		)
		set(Sqxc_LIBRARIES
		    ${Sqxc_LIBRARIES}
		    ${MYSQL_LIBRARIES}
		)
		# --- Visual Studio 2015 and later ---
		if (MSVC AND NOT (MSVC_VERSION LESS 1900))
			set(Sqxc_LIBRARIES
			    ${Sqxc_LIBRARIES}
			    legacy_stdio_definitions.lib
			)
		endif ()
		# --- debug message ---
		message(STATUS "MySQL include dirs: ${MYSQL_INCLUDE_DIRS}")
		message(STATUS "MySQL libraries: ${MYSQL_LIBRARIES}")
	endif (MYSQL_FOUND)

	# --- PostgreSQL ---
	if (PostgreSQL_FOUND)
		set(Sqxc_INCLUDE_DIRS
		    ${Sqxc_INCLUDE_DIRS}
		    ${PostgreSQL_INCLUDE_DIRS}
		)
		set(Sqxc_LIBRARIES
		    ${Sqxc_LIBRARIES}
		    ${PostgreSQL_LIBRARIES}
		)
		# --- debug message ---
		message(STATUS "PostgreSQL include dirs: ${PostgreSQL_INCLUDE_DIRS}")
		message(STATUS "PostgreSQL libraries: ${PostgreSQL_LIBRARIES}")
	endif (PostgreSQL_FOUND)

	# --- cJSON ---
	find_package(cJSON)
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
	endif (cJSON_FOUND)

	# --- json-c ---
	find_package(json-c)
	if (json-c_FOUND)
		set(JSONC_LIBRARIES json-c)
		set(Sqxc_INCLUDE_DIRS
		    ${Sqxc_INCLUDE_DIRS}
		    ${JSONC_INCLUDE_DIRS}
		)
		set(Sqxc_LIBRARIES
		    ${Sqxc_LIBRARIES}
		    ${JSONC_LIBRARIES}
		)
		# --- debug message ---
		message(STATUS "json-c include dirs: ${JSONC_INCLUDE_DIRS}")
		message(STATUS "json-c libraries: ${JSONC_LIBRARIES}")
	endif (json-c_FOUND)

endif (Sqxc_FOUND)

mark_as_advanced(Sqxc_INCLUDE_DIR Sqxc_LIBRARY)
