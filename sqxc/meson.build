sqxc_dependencies = []
sqxc_static_link_libs = []
sqxc_shared_link_libs = []

# --- 'sqxc' sources and headers ---
sources = [
    'SqArray.c',
    'SqPtrArray.c',
    'SqStrArray.c',
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

    'SqArray.h',
    'SqPtrArray.h',
    'SqStrArray.h',
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
install_headers(headers, subdir: 'sqxc')

# --- 'sqxcpp' C++ sources and headers ---
sources_cpp = [
    'SqType-std-string.cpp',
]

headers_cpp = [
    'SqType-stl-cpp.h',
]
install_headers(headers_cpp, subdir: 'sqxc')

# --- thread ---
#if host_machine.system() != 'windows'
#	if thread.found() == true
#		sqxc_dependencies += thread
#	endif
#endif

# --- json-c ---
if jsonc.found() == true
	sources += ['SqxcJsonc.c']
	headers += ['SqxcJsonc.h']
	sqxc_dependencies += jsonc
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

# --- PostgreSQL ---
if postgresql.found() == true
	sources_postgresql = ['SqdbPostgre.c']
	headers_postgresql = ['SqdbPostgre.h']
	install_headers(headers_postgresql, subdir: 'sqxc')
endif

configure_file(input : 'config.h.in',
               output : 'config.h',
               install : true,
               install_dir : 'include' / 'sqxc',
               configuration : config_data
)

sqxc_inc = include_directories('./')

# --- 'sqxc' static library ---
sqxc_lib = static_library(
    'sqxc',
    sources,
    include_directories: sqxc_inc,
    dependencies: sqxc_dependencies,
    install: true
)
sqxc_static_link_libs += sqxc_lib

# --- 'sqxc' shared library ---
sqxc_shared_lib = shared_library(
    'sqxc',
    sources,
    include_directories: sqxc_inc,
    dependencies: sqxc_dependencies,
    version: sqxclib_version,
#    soversion: sqxclib_soversion,
    install: true
)
sqxc_shared_link_libs += sqxc_shared_lib

# --- 'sqxcpp' static library ---
sqxcpp_lib = static_library(
    'sqxcpp',
    sources_cpp,
    include_directories: sqxc_inc,
#    dependencies: sqxc_dependencies,
    install: true
)
sqxc_static_link_libs += sqxcpp_lib

# --- 'sqxcpp' shared library ---
sqxcpp_shared_lib = shared_library(
    'sqxcpp',
    sources_cpp,
    include_directories: sqxc_inc,
    link_with: sqxc_shared_lib,
#    dependencies: sqxc_dependencies,
    version: sqxclib_version,
#    soversion: sqxclib_soversion,
    install: true
)
sqxc_shared_link_libs += sqxcpp_shared_lib

# --- 'sqxc_sqlite' library ---
if sqlite.found() == true
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
    sqxc_dependencies += sqlite
    sqxc_static_link_libs += sqxc_sqlite_lib
    sqxc_shared_link_libs += sqxc_sqlite_shared_lib
endif

# --- 'sqxc_mysql' library ---
if mysql.found() == true
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
    sqxc_dependencies += mysql
    sqxc_static_link_libs += sqxc_mysql_lib
    sqxc_shared_link_libs += sqxc_mysql_shared_lib
endif

# --- 'sqxc_postgre' library ---
if postgresql.found() == true
	sqxc_postgresql_lib = static_library(
	    'sqxc_postgre',
	    sources_postgresql,
	    include_directories: sqxc_inc,
	    dependencies: postgresql,
	    install: true
	)
	sqxc_postgresql_shared_lib = shared_library(
	    'sqxc_postgre',
	    sources_postgresql,
	    include_directories: sqxc_inc,
	    link_with: sqxc_shared_lib,
	    dependencies: postgresql,
	    version: sqxclib_version,
#	    soversion: sqxclib_soversion,
	    install: true
	)
    sqxc_dependencies += postgresql
    sqxc_static_link_libs += sqxc_postgresql_lib
    sqxc_shared_link_libs += sqxc_postgresql_shared_lib
endif

# static dependency
sqxc = declare_dependency(
    include_directories: sqxc_inc,
    link_with: sqxc_static_link_libs,
    dependencies: sqxc_dependencies
)

# shared dependency
sqxc_shared = declare_dependency(
    include_directories: sqxc_inc,
    link_with: sqxc_shared_link_libs,
    dependencies: sqxc_dependencies
)
