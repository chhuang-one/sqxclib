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

# --- MySQL ---
if mysql_config.found()
	sources += ['SqdbMysql.c', 'Sqdb-migration.c']
	headers += ['SqdbMysql.h', 'Sqdb-migration.h']
	dependencies += mysql
endif

# --- SQLite ---
if sqlite.found() == true
	sources += ['SqdbSqlite.c', 'SqRelation-migration.c', 'SqRelation.c']
	headers += ['SqdbSqlite.h', 'SqRelation-migration.h']
	dependencies += sqlite
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
    dependencies : dependencies,
    install: true
)

# --- 'sqxcpp' static library ---
sqxcpp_lib = static_library(
    'sqxcpp',
    sources_cpp,
    include_directories: sqxc_inc,
    dependencies : dependencies,
    install: true
)

# --- 'sqxc' shared library ---
sqxc_shared_lib = shared_library(
    'sqxc',
    sources,
    include_directories: sqxc_inc,
    dependencies : dependencies,
#    version : sqxclib_version,
#    soversion : sqxclib_soversion,
    install: true
)

# --- 'sqxcpp' shared library ---
sqxcpp_shared_lib = shared_library(
    'sqxcpp',
    sources_cpp,
    include_directories: sqxc_inc,
    link_with : sqxc_shared_lib,
    dependencies : dependencies,
    install: true
)

# static dependency
sqxc = declare_dependency(
    include_directories : sqxc_inc,
    link_with : [sqxcpp_lib, sqxc_lib],
    dependencies : dependencies
)

# shared dependency
sqxc_shared = declare_dependency(
    include_directories : sqxc_inc,
    link_with : [sqxcpp_shared_lib, sqxc_shared_lib],
    dependencies : dependencies
)
