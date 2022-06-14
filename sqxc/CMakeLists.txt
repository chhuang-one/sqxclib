# --- 'sqxc' sources and headers ---
set(SOURCES
    SqPtrArray.c
    SqBuffer.c
#    SqThread.c
    SqUtil.c
    SqType.c
    SqType-built-in.c
    SqType-PtrArray.c
    SqType-fake.c
    SqEntry.c
    SqColumn.c
    SqTable.c
    SqJoint.c
    SqSchema.c
    SqStorage.c
    SqStorage-query.c
    SqQuery.c
    Sqdb.c
    Sqxc.c
    SqxcValue.c
    SqxcSql.c
)

set(HEADERS
    sqxclib.h
    SqConfig.h
    SqDefine.h
    SqError.h
    SqPtrArray.h
    SqBuffer.h
#    SqThread.h
    SqUtil.h
    SqType.h
    SqEntry.h
    SqColumn.h
    SqTable.h
    SqJoint.h
    SqSchema.h
    SqSchema-macro.h
    SqStorage.h
    SqQuery.h
    SqQuery-macro.h
    SqRelation.h
    Sqdb.h
    Sqxc.h
    SqxcValue.h
    SqxcSql.h
)

set(SOURCES_CPP
    SqType-std-string.cpp
)
set(HEADERS_CPP
    SqType-stl-cpp.h
)

# --- json-c ---
if (JSONC_INCLUDE_DIRS)
	set(SOURCES
	    ${SOURCES}
	    SqxcJsonc.c
	)
	set(HEADERS
	    ${HEADERS}
	    SqxcJsonc.h
	)
endif()

# --- SQLite ---
if (SQLite3_INCLUDE_DIRS)
	set(SOURCES
	    ${SOURCES}
	    SqdbSqlite.c
	    SqRelation-migration.c
	    SqRelation.c
	)
	set(HEADERS
	    ${HEADERS}
	    SqdbSqlite.h
	    SqRelation-migration.h
#	    SqRelation.h
	)
endif()

# --- MySQL ---
if (MYSQL_FOUND)
	set(SOURCES
	    ${SOURCES}
	    SqdbMysql.c
	    Sqdb-migration.c
	)
	set(HEADERS
	    ${HEADERS}
	    SqdbMysql.h
	    Sqdb-migration.h
	)
endif()

# --- build library ---
include_directories(${SQXC_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}")
link_libraries(${SQXC_LIBRARIES})

# --- 'sqxc' library ---
add_library(sqxc
            ${SOURCES}
            ${HEADERS}
)
set_target_properties(sqxc
    PROPERTIES
#    VERSION        ${PROJECT_VERSION}
#    SOVERSION      ${SQXCLIB_SOVERSION}
    PUBLIC_HEADER "${HEADERS}"
)
# add 'sqxc' to SQXC_LIBRARIES
set(SQXC_LIBRARIES
    sqxc
    ${SQXC_LIBRARIES}
)
# install
install(
    TARGETS sqxc
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sqxc
)

# 'sqxc' static library
#if (BUILD_SHARED_LIBS)
#    add_library(sqxc-static
#                STATIC
#                ${SOURCES}
#                ${SOURCES_CPP}
#                ${HEADERS}
#                ${HEADERS_CPP}
#    )
#    set_target_properties(sqxc-static
#        PROPERTIES
#        OUTPUT_NAME sqxc
#    )
#endif(BUILD_SHARED_LIBS)

# --- 'sqxcpp' library ---
if (DEFINED CMAKE_CXX_COMPILER)
    add_library(sqxcpp
                ${SOURCES_CPP}
                ${HEADERS_CPP}
    )
    if (BUILD_SHARED_LIBS)
        target_link_libraries(sqxcpp sqxc)
    endif(BUILD_SHARED_LIBS)

    set_target_properties(sqxcpp
        PROPERTIES
#            VERSION        ${PROJECT_VERSION}
#            SOVERSION      ${SQXCLIB_SOVERSION}
            PUBLIC_HEADER "${HEADERS_CPP}"
    )
    # add 'sqxcpp' to SQXC_LIBRARIES
    set(SQXC_LIBRARIES
        sqxcpp
        ${SQXC_LIBRARIES}
    )
    # install
    install(
        TARGETS sqxcpp
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sqxc
    )
endif (CMAKE_CXX_COMPILER)

# set SQXC_INCLUDE_DIRS to PARENT_SCOPE
set(SQXC_INCLUDE_DIRS
    ${SQXC_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}"
    PARENT_SCOPE
)

# set SQXC_LIBRARIES to PARENT_SCOPE
set(SQXC_LIBRARIES
    ${SQXC_LIBRARIES}
    PARENT_SCOPE
)
