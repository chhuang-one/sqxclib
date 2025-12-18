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

# - Try to find sqxclib
# Project site: https://github.com/chhuang-one/sqxclib/
#               https://gitee.com/chhuang-one/sqxclib/

# Sqxc_FOUND        - If false, do not try to use sqxclib.
# Sqxc_INCLUDE_DIRS - Where to find sqxclib.h, etc.
# Sqxc_LIBRARIES    - The libraries to link against.

find_path(Sqxc_INCLUDE_DIR
    NAMES "sqxclib.h"
    PATHS "/usr/include/sqxc"
          "/usr/local/include/sqxc"
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
	    ${Sqxc_INCLUDE_DIR}/app
	    ${Sqxc_INCLUDE_DIR}/support
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
		endif ()

		unset(CurLibPath CACHE)
		unset(CurLibPath)
	endforeach()
endif ()

mark_as_advanced(Sqxc_INCLUDE_DIR Sqxc_LIBRARY)
