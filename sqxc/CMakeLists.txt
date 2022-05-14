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

# --- sqxc ---
include_directories(${SQXC_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}")
link_libraries(${SQXC_LIBRARIES})

add_library(sqxc
            STATIC
            ${SOURCES}
            ${HEADERS}
)

add_library(sqxcpp
            STATIC
            ${SOURCES_CPP}
            ${HEADERS_CPP}
)

# set SQXC_INCLUDE_DIRS to PARENT_SCOPE
set(SQXC_INCLUDE_DIRS
    ${SQXC_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}"
    PARENT_SCOPE
)

# set SQXC_LIBRARIES to PARENT_SCOPE
set(SQXC_LIBRARIES
    sqxcpp
    sqxc
    ${SQXC_LIBRARIES}
    PARENT_SCOPE
)
