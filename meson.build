project('sqxc', 'c', 'cpp',
#        default_options : ['c_std=c99', 'cpp_std=c++11'],
        version: '0.1.1')

add_global_arguments('-DDEBUG', language : ['c', 'cpp'])

jsonc = dependency('json-c', required: true)

# SQLite
sqlite = dependency('sqlite3', required: false)
if sqlite.found() == true
  has_sqlite = true
  add_project_arguments('-DHAS_SQLITE', language : 'c')
else
  has_sqlite = false
endif

# MySQL, MariaDB
mysql_config = find_program('mariadb_config', required: false)
if mysql_config.found() == false
  mysql_config = find_program('mysql_config', required: false)
endif
if mysql_config.found()
  mysql = declare_dependency(compile_args: run_command([mysql_config, '--cflags']).stdout().split(),
                             link_args: run_command([mysql_config, '--libs']).stdout().split() )
  has_mysql = true
else
  has_mysql = false
endif

subdir('sqxc')
subdir('tests')
subdir('examples')

