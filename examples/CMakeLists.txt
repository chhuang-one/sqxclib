include_directories(${SQXC_INCLUDE_DIRS})
link_libraries(${SQXC_LIBRARIES})

# bili2mp4 can't compile with MSVC because it uses readdir()...etc functions.
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	if (JSONC_FOUND)
		add_executable(bili2mp4  bili2mp4.c)
	endif (JSONC_FOUND)
endif ()

add_executable(storage-c99  storage-c99.c)

if (DEFINED CMAKE_CXX_COMPILER)
	add_executable(storage-cxx  storage-cxx.cpp)

	add_executable(storage-row  storage-row.cpp)
	target_include_directories(storage-row  PUBLIC  ${SQXCSUPPORT_INCLUDE_DIRS})
	target_link_libraries(storage-row  ${SQXCSUPPORT_LIBRARIES})
endif (DEFINED CMAKE_CXX_COMPILER)

add_executable(myapp-c99  myapp-c99.c)
target_include_directories(myapp-c99  PUBLIC  ${SQXCAPP_INCLUDE_DIRS})
target_link_libraries(myapp-c99  ${SQXCAPP_LIBRARIES})

if (DEFINED CMAKE_CXX_COMPILER)
	add_executable(myapp-cxx  myapp-cxx.cpp)
	target_include_directories(myapp-cxx  PUBLIC  ${SQXCAPP_INCLUDE_DIRS})
	target_link_libraries(myapp-cxx  ${SQXCAPP_LIBRARIES})

	# --- json-c is required
	if (JSONC_FOUND)
		add_executable(xc_json_file  xc_json_file.cpp)
		target_include_directories(xc_json_file  PUBLIC  ${SQXCSUPPORT_INCLUDE_DIRS})
		target_link_libraries(xc_json_file  ${SQXCSUPPORT_LIBRARIES})
	endif (JSONC_FOUND)
endif (DEFINED CMAKE_CXX_COMPILER)
