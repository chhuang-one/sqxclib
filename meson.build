project('sqxclib', 'c', 'cpp',
        default_options : ['c_std=c99', 'cpp_std=c++11'],
        version: '0.4')

add_global_arguments('-DDEBUG', language : ['c', 'cpp'])
# add_project_arguments('-DHAVE_CONFIG_H', language : ['c', 'cpp'])

config_data = configuration_data()

# json-c
jsonc = dependency('json-c', required: false)
if jsonc.found() == true
	config_data.set('have_jsonc',  '1')
else
	config_data.set('have_jsonc',  '0')
endif

# SQLite
sqlite = dependency('sqlite3', required: false)
if sqlite.found() == true
	config_data.set('have_sqlite', '1')
else
	config_data.set('have_sqlite', '0')
endif

# MySQL, MariaDB
mysql_config = find_program('mariadb_config', required: false)
if mysql_config.found() == false
	mysql_config = find_program('mysql_config', required: false)
endif
if mysql_config.found()
	mysql = declare_dependency(compile_args: run_command([mysql_config, '--cflags']).stdout().split(),
	                           link_args: run_command([mysql_config, '--libs']).stdout().split() )
	config_data.set('have_mysql',  '1')
else
	config_data.set('have_mysql',  '0')
endif

subdir('sqxc')
subdir('tests')
subdir('examples')

