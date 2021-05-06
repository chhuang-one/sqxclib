cmake_minimum_required(VERSION 3.4.1)

project(sqxc-toplevel)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/"
)

set(CMAKE_C_FLAGS       "${CMAKE_C_FLAGS} -DDEBUG")

# use find_package(PkgConfig REQUIRED) before use pkg-config module
find_package(PkgConfig REQUIRED)

# pkg-config module
pkg_check_modules(SQLITE3 sqlite3)
pkg_check_modules(JSONC json-c)

# find MySQL
find_package(MySQL)

add_subdirectory(sqxc)
