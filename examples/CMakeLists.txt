# include_directories(${Sqxc_INCLUDE_DIRS})
# link_libraries(${Sqxc_LIBRARIES})

# bili2mp4
# --- cJSON or json-c is required
if (cjson_FOUND OR json-c_FOUND)
	# If you want to build bili2mp4 in Visual Studio, download direct.h for MSVC form internet.
	if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		# find direct.h in current directory.
		find_path(MSVC_DIRECT_H
		    NAMES dirent.h
		    PATHS ${CMAKE_CURRENT_SOURCE_DIR}
		)
		if (DEFINED MSVC_DIRECT_H)
			message(STATUS "The direct.h file was found in examples directory ${CMAKE_CURRENT_SOURCE_DIR}")
			message(STATUS "This will build bili2mp4 for MSVC.")
			add_executable(bili2mp4  bili2mp4.c)
			target_include_directories(bili2mp4  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}  ${Sqxc_INCLUDE_DIRS})
		endif ()
	else ()
		add_executable(bili2mp4  bili2mp4.c)
		target_include_directories(bili2mp4  PUBLIC  ${Sqxc_INCLUDE_DIRS})
	endif ()
	target_link_libraries(bili2mp4  ${Sqxc_LIBRARIES})
endif (cjson_FOUND OR json-c_FOUND)

add_executable(storage-c99  storage-c99.c)
target_include_directories(storage-c99  PUBLIC  ${Sqxc_INCLUDE_DIRS})
target_link_libraries(storage-c99  ${Sqxc_LIBRARIES})

if (DEFINED CMAKE_CXX_COMPILER)
	add_executable(storage-cxx  storage-cxx.cpp)
	target_include_directories(storage-cxx  PUBLIC  ${Sqxc_INCLUDE_DIRS})
	target_link_libraries(storage-cxx  ${Sqxc_LIBRARIES})

	add_executable(storage-row  storage-row.cpp)
	target_include_directories(storage-row  PUBLIC  ${SqxcSupport_INCLUDE_DIRS})
	target_link_libraries(storage-row  ${SqxcSupport_LIBRARIES})
endif (DEFINED CMAKE_CXX_COMPILER)

add_executable(myapp-c99  myapp-c99.c)
target_include_directories(myapp-c99  PUBLIC  ${SqxcApp_INCLUDE_DIRS})
target_link_libraries(myapp-c99  ${SqxcApp_LIBRARIES})

if (DEFINED CMAKE_CXX_COMPILER)
	add_executable(myapp-cxx  myapp-cxx.cpp)
	target_include_directories(myapp-cxx  PUBLIC  ${SqxcApp_INCLUDE_DIRS})
	target_link_libraries(myapp-cxx  ${SqxcApp_LIBRARIES})

	# --- cJSON or json-c is required
	if (cjson_FOUND OR json-c_FOUND)
		add_executable(xc_json_file  xc_json_file.cpp)
		target_include_directories(xc_json_file  PUBLIC  ${SqxcSupport_INCLUDE_DIRS})
		target_link_libraries(xc_json_file  ${SqxcSupport_LIBRARIES})
	endif (cjson_FOUND OR json-c_FOUND)
endif (DEFINED CMAKE_CXX_COMPILER)
