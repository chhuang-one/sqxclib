project('user-project', 'c', 'cpp', version: '1.0')

# add_project_arguments('-DNDEBUG', language : ['c', 'cpp'])

# --- sqxclib ---
sqxclib = dependency('sqxclib', required: false)

# --- MySQL, MariaDB ---
mariadb_pc_found = false
mysqlclient_pc_found = false
mysql = dependency('mysqlclient', required: false)
if mysql.found() == true
	mysqlclient_pc_found = true
else
	mysql = dependency('mariadb', required: false)
	if mysql.found() == true
		mariadb_pc_found = true
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
	endif
endif

# add mysql to dependency if mysqlclient.pc (or mariadb.pc) not found
dependencies = [sqxclib]
if mysql.found() == true and (mariadb_pc_found == false and mysqlclient_pc_found == false)
	dependencies += mysql
endif

sqxc = declare_dependency(
    dependencies : dependencies
)

# --- subdirectory ---
subdir('sqxcapp')
subdir('src')
