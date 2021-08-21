set(SOURCES
    SqPtrArray.c
    SqBuffer.c
    SqUtil.c
    SqType.c
    SqType-built-in.c
    SqType-PtrArray.c
    SqType-fake.c
    SqEntry.c
    SqTable.c
    SqJoint.c
    SqSchema.c
    SqStorage.c
    SqStorage-query.c
    SqQuery.c
    SqMigration.c
    Sqdb.c
    Sqxc.c
    SqxcUnknown.c
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
    SqUtil.h
    SqType.h
    SqEntry.h
    SqTable.h
    SqJoint.h
    SqSchema.h
    SqSchema-macro.h
    SqStorage.h
    SqQuery.h
    SqQuery-macro.h
    SqMigration.h
    SqRelation.h
    Sqdb.h
    Sqxc.h
    SqxcUnknown.h
    SqxcValue.h
    SqxcSql.h
)

set(SOURCES_CPP
    SqType-std-string.cpp
)
set(HEADERS_CPP
    SqType-stl-cpp.h
)

set(SOURCES_TEST
    SqdbEmpty.c
    SqxcEmpty.c
)
set(HEADERS_TEST
    SqdbEmpty.h
    SqxcEmpty.h
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
include_directories(${SQXC_INCLUDE_DIRS})
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

add_library(sqxctest
            STATIC
            ${SOURCES_TEST}
            ${HEADERS_TEST}
)
