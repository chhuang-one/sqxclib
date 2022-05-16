include_directories(${SQXC_INCLUDE_DIRS})
link_libraries(${SQXC_LIBRARIES})

add_executable(test-container-cxx  test-container-cxx.cpp)

add_executable(test-cxx  test-cxx.cpp)
target_include_directories(test-cxx  PUBLIC  ${SQXCSUPPORT_INCLUDE_DIRS})
target_link_libraries(test-cxx  ${SQXCSUPPORT_LIBRARIES})

add_executable(test-migration  test-migration.c)

add_executable(test-query  test-query.c)

add_executable(test-sqxc  test-sqxc.c)
target_include_directories(test-sqxc  PUBLIC  ${SQXCSUPPORT_INCLUDE_DIRS})
target_link_libraries(test-sqxc  ${SQXCSUPPORT_LIBRARIES})

add_executable(test-storage  test-storage.c)

add_executable(test-util  test-util.c)
target_include_directories(test-util  PUBLIC  ${SQXCSUPPORT_INCLUDE_DIRS})
target_link_libraries(test-util  ${SQXCSUPPORT_LIBRARIES})

# --- test ---
add_test(NAME test-container-cxx COMMAND test-container-cxx)
add_test(NAME test-cxx COMMAND test-cxx)
add_test(NAME test-migration COMMAND test-migration)
add_test(NAME test-query COMMAND test-query)
add_test(NAME test-sqxc COMMAND test-sqxc)
add_test(NAME test-storage COMMAND test-storage)
add_test(NAME test-util COMMAND test-util)
