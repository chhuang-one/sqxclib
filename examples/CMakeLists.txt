set(EXAMPLE_INCLUDE_DIRS
    ${SQXC_INCLUDE_DIRS}
)
set(EXAMPLE_LIBRARIES
    ${SQXC_LIBRARIES}
)

# --- json-c ---
if (JSONC_INCLUDE_DIRS)
	set(EXAMPLE_INCLUDE_DIRS
		${EXAMPLE_INCLUDE_DIRS}
		${JSONC_INCLUDE_DIRS}
	)
	set(EXAMPLE_LIBRARIES
		${EXAMPLE_LIBRARIES}
		${JSONC_LIBRARIES}
	)
endif()

# --- SQLite ---
if (SQLite3_FOUND)
	set(EXAMPLE_INCLUDE_DIRS
	    ${EXAMPLE_INCLUDE_DIRS}
	    ${SQLite3_INCLUDE_DIRS}
	)
	set(EXAMPLE_LIBRARIES
	    ${EXAMPLE_LIBRARIES}
	    ${SQLite3_LIBRARIES}
	)
endif()

# --- MySQL ---
if (MYSQL_FOUND)
	set(EXAMPLE_INCLUDE_DIRS
	    ${EXAMPLE_INCLUDE_DIRS}
	    ${MYSQL_INCLUDE_DIRS}
	)
	set(EXAMPLE_LIBRARIES
	    ${EXAMPLE_LIBRARIES}
	    ${MYSQL_LIBRARIES}
	)
endif()


include_directories(${EXAMPLE_INCLUDE_DIRS})
link_libraries(${EXAMPLE_LIBRARIES})

add_executable(storage-c99  storage-c99.c)

add_executable(storage-cxx  storage-cxx.cpp)
