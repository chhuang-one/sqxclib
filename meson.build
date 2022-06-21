project('sqxclib', 'c', 'cpp',
        default_options : ['c_std=c99', 'cpp_std=c++11'],
        version: '0.6.3')

# add_project_arguments('-DHAVE_CONFIG_H', language : ['c', 'cpp'])
# add_project_arguments('-DNDEBUG', language : ['c', 'cpp'])

sqxclib_version = meson.project_version()
sqxclib_soversion = 0

config_data = configuration_data()

# --- therad ---
if host_machine.system() == 'windows'
	config_data.set('SQXCLIB_HAVE_THREAD',  '1')
else
	thread = dependency('threads', required: false)
	if thread.found() == true
		config_data.set('SQXCLIB_HAVE_THREAD',  '1')
	else
		config_data.set('SQXCLIB_HAVE_THREAD',  '0')
	endif
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
mysql_config = find_program('mariadb_config', required: false)
if mysql_config.found() == false
	mysql_config = find_program('mysql_config', required: false)
endif
if mysql_config.found()
	mysql = declare_dependency(compile_args: run_command([mysql_config, '--cflags']).stdout().split(),
	                           link_args: run_command([mysql_config, '--libs']).stdout().split() )
	config_data.set('SQXCLIB_HAVE_MYSQL',  '1')
else
	config_data.set('SQXCLIB_HAVE_MYSQL',  '0')
endif

config_data.set('SQXCLIB_INSTALL_PREFIX', get_option('prefix'))
config_data.set('SQXCLIB_VERSION', sqxclib_version)
# share/pkgconfig/sqxclib.pc
configure_file(input : 'sqxclib.pc.in',
               output : 'sqxclib.pc',
               install : true,
               install_dir : get_option('datadir') / 'pkgconfig',
               configuration : config_data)

subdir('sqxc')
subdir('sqxcsupport')
subdir('sqxcapp')
subdir('tests')
subdir('examples')

