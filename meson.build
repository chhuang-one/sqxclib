# Don't specify version of the C or C++ language standard.
# Note: sqxclib code still compile with C99 and C++11 currently.
project('sqxclib',
    'c', 'cpp',
    default_options : [
#        'c_std=c99',
#        'cpp_std=c++11',
    ],
    version: '1.1.1'
)

# add_project_arguments('-DHAVE_CONFIG_H', language : ['c', 'cpp'])
# add_project_arguments('-DNDEBUG', language : ['c', 'cpp'])

sqxclib_version = meson.project_version()
sqxclib_soversion = 0

config_data = configuration_data()

# --- Sqxc include dirs ---
sqxc_inc = include_directories('./')

# --- cJSON ---
cjson = dependency('libcjson', required: false)
if cjson.found() == true
	config_data.set('SQXCLIB_HAVE_CJSON',  '1')
else
	config_data.set('SQXCLIB_HAVE_CJSON',  '0')
endif

# --- json-c ---
jsonc = dependency('json-c', required: false)
if jsonc.found() == true
	config_data.set('SQXCLIB_HAVE_JSONC',  '1')
else
	config_data.set('SQXCLIB_HAVE_JSONC',  '0')
endif

# --- SQLite ---
sqlite = dependency('sqlite3', required: false)
if sqlite.found() == true
	config_data.set('SQXCLIB_HAVE_SQLITE', '1')
else
	config_data.set('SQXCLIB_HAVE_SQLITE', '0')
endif

# --- MySQL, MariaDB ---
mariadb_pc_found = false
mysqlclient_pc_found = false
mysql = dependency('mysqlclient', required: false)
if mysql.found() == true
	mysqlclient_pc_found = true
	config_data.set('SQXCLIB_HAVE_MYSQL',  '1')
else
	mysql = dependency('mariadb', required: false)
	if mysql.found() == true
		mariadb_pc_found = true
		config_data.set('SQXCLIB_HAVE_MYSQL',  '1')
	endif
endif
# find MySQL if mysqlclient.pc (or mariadb.pc) not found
if mysql.found() == false
	mysql_config = find_program('mysql_config', required: false)
	if mysql_config.found() == false
		mysql_config = find_program('mariadb_config', required: false)
	endif
	if mysql_config.found() == true
		mysql = declare_dependency(compile_args: run_command([mysql_config, '--cflags']).stdout().split(),
		                           link_args: run_command([mysql_config, '--libs']).stdout().split() )
		config_data.set('SQXCLIB_HAVE_MYSQL',  '1')
	else
		config_data.set('SQXCLIB_HAVE_MYSQL',  '0')
	endif
endif

# --- PostgreSQL ---
postgresql = dependency('libpq', required: false)
if postgresql.found() == true
	config_data.set('SQXCLIB_HAVE_POSTGRESQL', '1')
else
	config_data.set('SQXCLIB_HAVE_POSTGRESQL', '0')
endif

# --- pkg-config metadata file --- share/pkgconfig/sqxclib.pc
config_data.set('SQXCLIB_PC_PREFIX', get_option('prefix'))
config_data.set('SQXCLIB_PC_VERSION', sqxclib_version)

config_data.set('SQXCLIB_PC_REQUIRE_CJSON', '')
config_data.set('SQXCLIB_PC_REQUIRE_JSONC', '')
config_data.set('SQXCLIB_PC_REQUIRE_SQLITE', '')
config_data.set('SQXCLIB_PC_REQUIRE_MYSQL', '')
config_data.set('SQXCLIB_PC_REQUIRE_POSTGRESQL', '')
config_data.set('SQXCLIB_PC_LIB_SQLITE', '')
config_data.set('SQXCLIB_PC_LIB_MYSQL', '')
config_data.set('SQXCLIB_PC_LIB_POSTGRESQL', '')
if cjson.found() == true
	config_data.set('SQXCLIB_PC_REQUIRE_CJSON', 'libcjson')
endif
if jsonc.found() == true
	config_data.set('SQXCLIB_PC_REQUIRE_JSONC', 'json-c')
endif
if sqlite.found() == true
	config_data.set('SQXCLIB_PC_REQUIRE_SQLITE', 'sqlite3')
	config_data.set('SQXCLIB_PC_LIB_SQLITE', '-lsqxc_sqlite')
endif
if mysql.found() == true
	# MySQL pkg-config name: mysqlclient or mariadb
	if mysqlclient_pc_found == true
		config_data.set('SQXCLIB_PC_REQUIRE_MYSQL', 'mysqlclient')
	elif mariadb_pc_found == true
		config_data.set('SQXCLIB_PC_REQUIRE_MYSQL', 'mariadb')
	endif
	config_data.set('SQXCLIB_PC_LIB_MYSQL', '-lsqxc_mysql')
endif
if postgresql.found() == true
	config_data.set('SQXCLIB_PC_REQUIRE_POSTGRESQL', 'libpq')
	config_data.set('SQXCLIB_PC_LIB_POSTGRESQL', '-lsqxc_postgre')
endif
# has c++ compiler
config_data.set('SQXCLIB_PC_LIB_CXX', '-lsqxcxx')

configure_file(input : 'sqxclib.pc.in',
               output : 'sqxclib.pc',
               install : true,
               install_dir : get_option('datadir') / 'pkgconfig',
               configuration : config_data)

# install project-template
install_subdir('project-template', install_dir : get_option('datadir') / 'sqxclib')

# --- subdirectory ---
subdir('sqxc')
subdir('sqxc/support')
subdir('sqxc/app')
subdir('sqapp')
subdir('tests')
subdir('examples')

