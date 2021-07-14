cmake_minimum_required(VERSION 3.14)

project(sqxclib)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/"
)

set(CMAKE_C_FLAGS    "${CMAKE_C_FLAGS} -DDEBUG")
# set(CMAKE_C_FLAGS    "${CMAKE_C_FLAGS} -DHAVE_CONFIG_H")

# use find_package(PkgConfig REQUIRED) before use pkg-config module
find_package(PkgConfig REQUIRED)

# pkg-config module
pkg_check_modules(JSONC json-c)

# find SQLite3
find_package(SQLite3)

# find MySQL
find_package(MySQL)


# --- config.h ---
if (JSONC_INCLUDE_DIRS)
	set(have_jsonc     1)
else()
	set(have_jsonc     0)
endif()

if (SQLite3_INCLUDE_DIRS)
	set(have_sqlite    1)
else()
	set(have_sqlite    0)
endif()

if (MYSQL_FOUND)
	set(have_mysql     1)
else()
	set(have_mysql     0)
endif()

configure_file("${CMAKE_SOURCE_DIR}/sqxc/config.h.in"
               "${CMAKE_SOURCE_DIR}/sqxc/config.h")


# --- sqxc ---
set(SQXC_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/sqxc"
)
set(SQXC_LIBRARIES
    sqxc
    sqxcpp
    sqxctest
)

add_subdirectory(sqxc)
add_subdirectory(examples)
