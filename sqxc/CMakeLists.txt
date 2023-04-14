# --- 'sqxc' sources and headers ---
set(SOURCES
    SqArray.c
    SqPtrArray.c
    SqStrArray.c
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
    Sqdb-migration.c    # Most of the SQL products may use this (exclude SQLite)
    Sqxc.c
    SqxcValue.c
    SqxcSql.c
)

set(HEADERS
    sqxclib.h
    SqConfig.h
    SqDefine.h
    SqError.h
    SqArray.h
    SqPtrArray.h
    SqStrArray.h
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
    SqQuery-proxy.h
    SqQuery-macro.h
    SqRelation.h
    Sqdb.h
    Sqdb-migration.h    # Most of the SQL products may use this (exclude SQLite)
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
if (JSONC_FOUND)    # if (JSONC_INCLUDE_DIRS)
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
if (SQLite3_FOUND)    #if (SQLite3_INCLUDE_DIRS)
	set(SOURCES_SQLITE
	    SqdbSqlite.c
	    SqRelation.c
	    SqRelation-migration.c
	)
	set(HEADERS_SQLITE
	    SqdbSqlite.h
#	    SqRelation.h
	    SqRelation-migration.h
	)
endif()

# --- MySQL ---
if (MYSQL_FOUND)
	set(SOURCES_MYSQL
	    SqdbMysql.c
	)
	set(HEADERS_MYSQL
	    SqdbMysql.h
	)
endif()

# --- PostgreSQL ---
if (PostgreSQL_FOUND)
	set(SOURCES_POSTGRESQL
	    SqdbPostgre.c
	)
	set(HEADERS_POSTGRESQL
	    SqdbPostgre.h
	)
endif()

# --- build ---
include_directories(${SQXC_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}")

# --- 'sqxc' library ---
add_library(sqxc
            ${SOURCES}
            ${HEADERS}
)
set_target_properties(sqxc
    PROPERTIES
    VERSION        ${PROJECT_VERSION}
#    SOVERSION      ${SQXCLIB_SOVERSION}
    PUBLIC_HEADER "${HEADERS}"
)
if (BUILD_SHARED_LIBS)
	target_link_libraries(sqxc ${SQXC_LIBRARIES})
endif(BUILD_SHARED_LIBS)

# add 'sqxc' to SQXC_LIBRARIES
set(SQXC_LIBRARIES
    sqxc
    ${SQXC_LIBRARIES}
)
# configure file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h"
)
install(
    FILES config.h ${CMAKE_CURRENT_SOURCE_DIR}/config.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sqxc
)
# install header files
install(
    TARGETS sqxc
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sqxc
)

# 'sqxc' static library
#if (BUILD_SHARED_LIBS)
#	add_library(sqxc-static
#	            STATIC
#	            ${SOURCES}
#	            ${SOURCES_CPP}
#	            ${HEADERS}
#	            ${HEADERS_CPP}
#	)
#	set_target_properties(sqxc-static
#	    PROPERTIES
#	    OUTPUT_NAME sqxc
#	)
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
	    VERSION        ${PROJECT_VERSION}
#	    SOVERSION      ${SQXCLIB_SOVERSION}
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
endif (DEFINED CMAKE_CXX_COMPILER)

# --- 'sqxc_sqlite' library ---
if (SQLite3_FOUND)
	add_library(sqxc_sqlite
	            ${SOURCES_SQLITE}
	            ${HEADERS_SQLITE}
	)
	target_include_directories(sqxc_sqlite
	    PUBLIC
	    ${SQXC_INCLUDE_DIRS}
	    ${SQLite3_INCLUDE_DIRS}
	)
	if (BUILD_SHARED_LIBS)
		target_link_libraries(sqxc_sqlite
		    sqxc
		    ${SQLite3_LIBRARIES}
		)
	endif (BUILD_SHARED_LIBS)

	set_target_properties(sqxc_sqlite
	    PROPERTIES
	    VERSION        ${PROJECT_VERSION}
#	    SOVERSION      ${SQXCLIB_SOVERSION}
	    PUBLIC_HEADER "${HEADERS_SQLITE}"
	)
	# add 'sqxc_sqlite' to SQXC_LIBRARIES
	set(SQXC_LIBRARIES
	    sqxc_sqlite
	    ${SQXC_LIBRARIES}
	)
	# install
	install(
	    TARGETS sqxc_sqlite
	    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sqxc
	)
endif (SQLite3_FOUND)

# --- 'sqxc_mysql' library ---
if (MYSQL_FOUND)
	add_library(sqxc_mysql
	            ${SOURCES_MYSQL}
	            ${HEADERS_MYSQL}
	)
	target_include_directories(sqxc_mysql
	    PUBLIC
	    ${SQXC_INCLUDE_DIRS}
	    ${MYSQL_INCLUDE_DIRS}
	)
	if (BUILD_SHARED_LIBS)
		target_link_libraries(sqxc_mysql
		    sqxc
		    ${MYSQL_LIBRARIES}
		)
	endif (BUILD_SHARED_LIBS)

	set_target_properties(sqxc_mysql
	    PROPERTIES
	    VERSION        ${PROJECT_VERSION}
#	    SOVERSION      ${SQXCLIB_SOVERSION}
	    PUBLIC_HEADER "${HEADERS_MYSQL}"
	)
	# add 'sqxc_mysql' to SQXC_LIBRARIES
	set(SQXC_LIBRARIES
	    sqxc_mysql
	    ${SQXC_LIBRARIES}
	)
	# install
	install(
	    TARGETS sqxc_mysql
	    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sqxc
	)
endif (MYSQL_FOUND)

# --- 'sqxc_postgre' library ---
if (PostgreSQL_FOUND)
	add_library(sqxc_postgre
	            ${SOURCES_POSTGRESQL}
	            ${HEADERS_POSTGRESQL}
	)
	target_include_directories(sqxc_postgre
	    PUBLIC
	    ${SQXC_INCLUDE_DIRS}
	    ${PostgreSQL_INCLUDE_DIRS}
	)
	if (BUILD_SHARED_LIBS)
		target_link_libraries(sqxc_postgre
		    sqxc
		    ${PostgreSQL_LIBRARIES}
		)
	endif (BUILD_SHARED_LIBS)

	set_target_properties(sqxc_postgre
	    PROPERTIES
	    VERSION        ${PROJECT_VERSION}
#	    SOVERSION      ${SQXCLIB_SOVERSION}
	    PUBLIC_HEADER "${HEADERS_POSTGRESQL}"
	)
	# add 'sqxc_postgre' to SQXC_LIBRARIES
	set(SQXC_LIBRARIES
	    sqxc_postgre
	    ${SQXC_LIBRARIES}
	)
	# install
	install(
	    TARGETS sqxc_postgre
	    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/sqxc
	)
endif (PostgreSQL_FOUND)

# update SQXC_INCLUDE_DIRS and SQXC_LIBRARIES
if (SQLite3_FOUND)
	set(SQXC_INCLUDE_DIRS
	    ${SQXC_INCLUDE_DIRS}
	    ${SQLite3_INCLUDE_DIRS}
	)
	set(SQXC_LIBRARIES
	    ${SQXC_LIBRARIES}
	    ${SQLite3_LIBRARIES}
	)
endif (SQLite3_FOUND)

if (MYSQL_FOUND)
	set(SQXC_INCLUDE_DIRS
	    ${SQXC_INCLUDE_DIRS}
	    ${MYSQL_INCLUDE_DIRS}
	)
	set(SQXC_LIBRARIES
	    ${SQXC_LIBRARIES}
	    ${MYSQL_LIBRARIES}
	)
	# --- Visual Studio 2015 and later ---
	if (MSVC AND NOT (MSVC_VERSION LESS 1900))
		set(SQXC_LIBRARIES
		    ${SQXC_LIBRARIES}
		    legacy_stdio_definitions.lib
		)
	endif ()
endif (MYSQL_FOUND)

if (PostgreSQL_FOUND)
	set(SQXC_INCLUDE_DIRS
	    ${SQXC_INCLUDE_DIRS}
	    ${PostgreSQL_INCLUDE_DIRS}
	)
	set(SQXC_LIBRARIES
	    ${SQXC_LIBRARIES}
	    ${PostgreSQL_LIBRARIES}
	)
endif (PostgreSQL_FOUND)

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
