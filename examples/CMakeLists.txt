include_directories(${SQXC_INCLUDE_DIRS})
link_libraries(${SQXC_LIBRARIES})

add_executable(storage-c99  storage-c99.c)

add_executable(storage-cxx  storage-cxx.cpp)

add_executable(myapp-c99  myapp-c99.c)
target_include_directories(myapp-c99  PUBLIC  ${SQXCAPP_INCLUDE_DIRS})
target_link_libraries(myapp-c99  ${SQXCAPP_LIBRARIES})

add_executable(myapp-cxx  myapp-cxx.cpp)
target_include_directories(myapp-cxx  PUBLIC  ${SQXCAPP_INCLUDE_DIRS})
target_link_libraries(myapp-cxx  ${SQXCAPP_LIBRARIES})

# --- json-c is required
if (JSONC_INCLUDE_DIRS)
add_executable(xc_json_file  xc_json_file.cpp)
target_include_directories(xc_json_file  PUBLIC  ${SQXCSUPPORT_INCLUDE_DIRS})
target_link_libraries(xc_json_file  sqxctest  ${SQXC_LIBRARIES})
endif()
