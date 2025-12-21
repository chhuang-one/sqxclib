# include_directories(${Sqxc_INCLUDE_DIRS})
# link_libraries(${Sqxc_LIBRARIES})

if (DEFINED CMAKE_CXX_COMPILER)
	add_executable(test-container-cxx  test-container-cxx.cpp)
	target_include_directories(test-container-cxx  PUBLIC  ${Sqxc_INCLUDE_DIRS})
	target_link_libraries(test-container-cxx  ${Sqxc_LIBRARIES})

	add_executable(test-cxx  test-cxx.cpp)
	target_include_directories(test-cxx  PUBLIC  ${SqxcSupport_INCLUDE_DIRS})
	target_link_libraries(test-cxx  ${SqxcSupport_LIBRARIES})
endif (DEFINED CMAKE_CXX_COMPILER)

add_executable(test-migration  test-migration.c)
target_include_directories(test-migration  PUBLIC  ${Sqxc_INCLUDE_DIRS})
target_link_libraries(test-migration  ${Sqxc_LIBRARIES})

add_executable(test-query  test-query.c)
target_include_directories(test-query  PUBLIC  ${Sqxc_INCLUDE_DIRS})
target_link_libraries(test-query  ${Sqxc_LIBRARIES})

add_executable(test-sqxc  test-sqxc.c)
target_include_directories(test-sqxc  PUBLIC  ${SqxcSupport_INCLUDE_DIRS})
target_link_libraries(test-sqxc  ${SqxcSupport_LIBRARIES})

add_executable(test-storage  test-storage.c)
target_include_directories(test-storage  PUBLIC  ${Sqxc_INCLUDE_DIRS})
target_link_libraries(test-storage  ${Sqxc_LIBRARIES})

add_executable(test-convert  test-convert.c)
target_include_directories(test-convert  PUBLIC  ${SqxcSupport_INCLUDE_DIRS})
target_link_libraries(test-convert  ${SqxcSupport_LIBRARIES})

# --- test ---
if (DEFINED CMAKE_CXX_COMPILER)
	add_test(NAME test-container-cxx COMMAND test-container-cxx)
	add_test(NAME test-cxx COMMAND test-cxx)
endif (DEFINED CMAKE_CXX_COMPILER)

add_test(NAME test-migration COMMAND test-migration)
add_test(NAME test-query COMMAND test-query)
add_test(NAME test-sqxc COMMAND test-sqxc)
add_test(NAME test-storage COMMAND test-storage)
add_test(NAME test-convert COMMAND test-convert)
