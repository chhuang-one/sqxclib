cmake_minimum_required(VERSION 3.14)

project(user-project VERSION 1.0)

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
	endif ()
endif (DEFINED CMAKE_CXX_COMPILER)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake"
)

# set(CMAKE_C_FLAGS    "${CMAKE_C_FLAGS} -DHAVE_CONFIG_H")

# find_package(PkgConfig REQUIRED) or
# find_package(PkgConfig) before use pkg-config module
find_package(PkgConfig)

# pkg-config module
if (PKG_CONFIG_FOUND)
	pkg_check_modules(Sqxc sqxclib)
endif (PKG_CONFIG_FOUND)

# find Sqxc if sqxclib.pc not found
if (NOT Sqxc_FOUND)
	find_package(Sqxc)
endif ()

# Exit processing of this CMakeLists.txt if Sqxc (sqxclib) not found.
if (NOT Sqxc_FOUND)
	message(FATAL_ERROR "Sqxc (sqxclib) not found.")
	return()
endif (NOT Sqxc_FOUND)

# --- subdirectory ---
add_subdirectory(sqapp)
add_subdirectory(src)

# --- debug message ---
message(STATUS "Sqxc (sqxclib) include dirs: ${Sqxc_INCLUDE_DIRS}")
message(STATUS "Sqxc (sqxclib) libraries: ${Sqxc_LIBRARIES}")
