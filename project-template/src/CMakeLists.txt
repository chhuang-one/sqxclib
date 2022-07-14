include_directories(${SQXC_INCLUDE_DIRS})
link_libraries(${SQXC_LIBRARIES})


# link shared library: 'sqxc'
add_executable(main  main.c)

# link shared library: 'sqxc' and 'sqxcapp'
add_executable(main-sqxcapp  main-sqxcapp.c)
target_include_directories(main-sqxcapp  PUBLIC  ${SQXCAPP_INCLUDE_DIRS})
target_link_libraries(main-sqxcapp  ${SQXCAPP_LIBRARIES})


if (DEFINED CMAKE_CXX_COMPILER)
	# link shared library: 'sqxc'.
	add_executable(main-cxx  main-cxx.cpp)

	# link shared library: 'sqxc' and 'sqxcapp'
	add_executable(main-sqxcapp-cxx  main-sqxcapp-cxx.cpp)
	target_include_directories(main-sqxcapp-cxx  PUBLIC  ${SQXCAPP_INCLUDE_DIRS})
	target_link_libraries(main-sqxcapp-cxx  ${SQXCAPP_LIBRARIES})
endif (DEFINED CMAKE_CXX_COMPILER)
