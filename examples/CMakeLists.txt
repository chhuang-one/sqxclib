include_directories(${SQXC_INCLUDE_DIRS})
link_libraries(${SQXC_LIBRARIES})

add_executable(storage-c99  storage-c99.c)

add_executable(storage-cxx  storage-cxx.cpp)
