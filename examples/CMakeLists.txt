# --- Visual Studio 2015 and later ---
if (MSVC AND NOT (MSVC_VERSION LESS 1900))
	set(SQXC_LIBRARIES
	    ${SQXC_LIBRARIES}
	    legacy_stdio_definitions.lib
	)
endif()

include_directories(${SQXC_INCLUDE_DIRS})
link_libraries(${SQXC_LIBRARIES})

add_executable(storage-c99  storage-c99.c)

add_executable(storage-cxx  storage-cxx.cpp)

add_executable(myapp-c99  myapp-c99.c)
target_include_directories(myapp-c99  PUBLIC  ${SQAPP_INCLUDE_DIRS})
target_link_libraries(myapp-c99  ${SQAPP_LIBRARIES})
