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
    SqRelation.c
    Sqdb.c
    Sqxc.c
    SqxcValue.c
    SqxcJsonc.c
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
    SqRelation.h
    Sqdb.h
    Sqxc.h
    SqxcValue.h
    SqxcJsonc.h
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


set(LOCAL_INCLUDE_DIRS
	${SQXC_INCLUDE_DIRS}
)
set(LOCAL_LIBRARIES
)

# --- json-c ---
if (JSONC_INCLUDE_DIRS)
	set(LOCAL_INCLUDE_DIRS
		${LOCAL_INCLUDE_DIRS}
		${JSONC_INCLUDE_DIRS}
	)
	set(LOCAL_LIBRARIES
		${LOCAL_LIBRARIES}
		${JSONC_LIBRARIES}
	)
endif()

# --- SQLite ---
if (SQLITE3_INCLUDE_DIRS)
	set(SOURCES
	    ${SOURCES}
	    SqdbSqlite.c
	    SqRelation-migration.c
	)
	set(HEADERS
	    ${HEADERS}
	    SqdbSqlite.h
	    SqRelation-migration.h
	)

	set(LOCAL_INCLUDE_DIRS
	    ${LOCAL_INCLUDE_DIRS}
	    ${SQLITE3_INCLUDE_DIRS}
	)
	set(LOCAL_LIBRARIES
	    ${LOCAL_LIBRARIES}
	    ${SQLITE3_LIBRARIES}
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

	set(LOCAL_INCLUDE_DIRS
	    ${LOCAL_INCLUDE_DIRS}
	    ${MYSQL_INCLUDE_DIR}
	)
	set(LOCAL_LIBRARIES
	    ${LOCAL_LIBRARIES}
	    ${MYSQL_LIBRARY}
	)
endif()


include_directories(${LOCAL_INCLUDE_DIRS})
link_libraries(${LOCAL_LIBRARIES})

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
