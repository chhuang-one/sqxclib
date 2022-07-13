# --- 'sqxc' sources and headers ---
sources = [
    'SqPtrArray.c',
    'SqBuffer.c',
#    'SqThread.c',
    'SqUtil.c',
    'SqType.c',
    'SqType-built-in.c',
    'SqType-PtrArray.c',
    'SqType-fake.c',
    'SqEntry.c',
    'SqColumn.c',
    'SqTable.c',
    'SqJoint.c',
    'SqSchema.c',
    'SqStorage.c',
    'SqStorage-query.c',
    'SqQuery.c',

    # Sqdb - Database interface
    'Sqdb.c',
    'Sqdb-migration.c',    # Most of the SQL products may use this (exclude SQLite)

    # Sqxc - Converter interface
    'Sqxc.c',
    'SqxcValue.c',
    'SqxcSql.c',
]

headers = [
    'sqxclib.h',
    'SqConfig.h',
    'SqDefine.h',
    'SqError.h',

    'SqPtrArray.h',
    'SqBuffer.h',
#    'SqThread.h',
    'SqUtil.h',
    'SqType.h',
    'SqEntry.h',
    'SqColumn.h',
    'SqTable.h',
    'SqJoint.h',
    'SqSchema.h', 'SqSchema-macro.h',
    'SqStorage.h',
    'SqQuery.h', 'SqQuery-proxy.h', 'SqQuery-macro.h',
    'SqRelation.h',

    # Sqdb - Database interface
    'Sqdb.h',
    'Sqdb-migration.h',    # Most of the SQL products may use this (exclude SQLite)

    # Sqxc - Converter interface
    'Sqxc.h',
    'SqxcValue.h',
    'SqxcSql.h',
]

# C++ sources & headers
sources_cpp = [
    'SqType-std-string.cpp',
]

headers_cpp = [
    'SqType-stl-cpp.h',
]

dependencies = []

#if host_machine.system() != 'windows'
#	if thread.found() == true
#		dependencies += thread
#	endif
#endif

# --- json-c ---
if jsonc.found() == true
	sources += ['SqxcJsonc.c']
	headers += ['SqxcJsonc.h']
	dependencies += jsonc
endif

# --- SQLite ---
if sqlite.found() == true
	sources_sqlite = ['SqdbSqlite.c',
	                  'SqRelation.c',
	                  'SqRelation-migration.c',
	]
	headers_sqlite = ['SqdbSqlite.h',
#	                  'SqRelation.h',
	                  'SqRelation-migration.h',
	]
	install_headers(headers_sqlite, subdir: 'sqxc')
endif

# --- MySQL ---
if mysql.found() == true
	sources_mysql = ['SqdbMysql.c']
	headers_mysql = ['SqdbMysql.h']
	install_headers(headers_mysql, subdir: 'sqxc')
endif

install_headers(headers, subdir: 'sqxc')
install_headers(headers_cpp, subdir: 'sqxc')

configure_file(input : 'config.h.in',
               output : 'config.h',
               install : true,
               install_dir : 'include' / 'sqxc',
               configuration : config_data)

sqxc_inc = include_directories('./')

# --- 'sqxc' static library ---
sqxc_lib = static_library(
    'sqxc',
    sources,
    include_directories: sqxc_inc,
    dependencies: dependencies,
    install: true
)

# --- 'sqxc' shared library ---
sqxc_shared_lib = shared_library(
    'sqxc',
    sources,
    include_directories: sqxc_inc,
    dependencies: dependencies,
    version: sqxclib_version,
#    soversion: sqxclib_soversion,
    install: true
)

# --- 'sqxcpp' static library ---
sqxcpp_lib = static_library(
    'sqxcpp',
    sources_cpp,
    include_directories: sqxc_inc,
#    dependencies: dependencies,
    install: true
)

# --- 'sqxcpp' shared library ---
sqxcpp_shared_lib = shared_library(
    'sqxcpp',
    sources_cpp,
    include_directories: sqxc_inc,
    link_with: sqxc_shared_lib,
#    dependencies: dependencies,
    version: sqxclib_version,
#    soversion: sqxclib_soversion,
    install: true
)

# --- 'sqxc_sqlite' library ---
if sqlite.found() == true
    dependencies += sqlite
	sqxc_sqlite_lib = static_library(
	    'sqxc_sqlite',
	    sources_sqlite,
	    include_directories: sqxc_inc,
	    dependencies: sqlite,
	    install: true
	)
	sqxc_sqlite_shared_lib = shared_library(
	    'sqxc_sqlite',
	    sources_sqlite,
	    include_directories: sqxc_inc,
	    link_with: sqxc_shared_lib,
	    dependencies: sqlite,
	    version: sqxclib_version,
#	    soversion: sqxclib_soversion,
	    install: true
	)
endif

# --- 'sqxc_mysql' library ---
if mysql.found() == true
    dependencies += mysql
	sqxc_mysql_lib = static_library(
	    'sqxc_mysql',
	    sources_mysql,
	    include_directories: sqxc_inc,
	    dependencies: mysql,
	    install: true
	)
	sqxc_mysql_shared_lib = shared_library(
	    'sqxc_mysql',
	    sources_mysql,
	    include_directories: sqxc_inc,
	    link_with: sqxc_shared_lib,
	    dependencies: mysql,
	    version: sqxclib_version,
#	    soversion: sqxclib_soversion,
	    install: true
	)
endif

# static dependency
sqxc = declare_dependency(
    include_directories: sqxc_inc,
    link_with: [sqxc_mysql_lib, sqxc_sqlite_lib, sqxcpp_lib, sqxc_lib],
    dependencies: dependencies
)

# shared dependency
sqxc_shared = declare_dependency(
    include_directories: sqxc_inc,
    link_with: [sqxc_mysql_shared_lib, sqxc_sqlite_shared_lib, sqxcpp_shared_lib, sqxc_shared_lib],
    dependencies: dependencies
)
