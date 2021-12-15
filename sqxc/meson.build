sources = ['SqPtrArray.c',
           'SqBuffer.c',
#           'SqThread.c',
           'SqUtil.c',
           'SqType.c',
           'SqType-built-in.c',
           'SqType-PtrArray.c',
           'SqType-fake.c',
           'SqEntry.c',
           'SqTable.c',
           'SqJoint.c',
           'SqSchema.c',
           'SqStorage.c',
           'SqStorage-query.c',
           'SqQuery.c',
           'SqMigration.c',

           # Sqdb - Database interface
           'Sqdb.c',

           # Sqxc - Converter interface
           'Sqxc.c',
           'SqxcUnknown.c',
           'SqxcValue.c',
           'SqxcSql.c',
          ]

headers = ['sqxclib.h',
           'SqConfig.h',
           'SqDefine.h',
           'SqError.h',

           'SqPtrArray.h',
           'SqBuffer.h',
#           'SqThread.h',
           'SqUtil.h',
           'SqType.h',
           'SqEntry.h',
           'SqTable.h',
           'SqJoint.h',
           'SqSchema.h', 'SqSchema-macro.h',
           'SqStorage.h',
           'SqQuery.h', 'SqQuery-macro.h',
           'SqMigration.h',
           'SqRelation.h',

           # Sqdb - Database interface
           'Sqdb.h',

           # Sqxc - Converter interface
           'Sqxc.h',
           'SqxcUnknown.h',
           'SqxcValue.h',
           'SqxcSql.h',
          ]

# C++ sources & headers

sources_cpp = ['SqType-std-string.cpp',
              ]

headers_cpp = ['SqType-stl-cpp.h',
              ]

# console sources & headers

sources_console = ['SqConsole.c',
                   'SqCommand.c',
                   'SqOption.c',
                  ]

headers_console = ['SqConsole.h',
                   'SqCommand.h',
                   'SqOption.h',
                  ]

# test sources & headers

sources_test = ['SqdbEmpty.c',
                'SqxcEmpty.c',
                'SqxcFile.c',
               ]

headers_test = ['SqdbEmpty.h',
                'SqxcEmpty.h',
                'SqxcFile.h',
               ]

dependencies = []

if host_machine.system() != 'windows'
	if thread.found() == true
		dependencies += thread
	endif
endif

if jsonc.found() == true
	sources += ['SqxcJsonc.c']
	headers += ['SqxcJsonc.h']
	dependencies += jsonc
endif

if mysql_config.found()
	sources += ['SqdbMysql.c', 'Sqdb-migration.c']
	headers += ['SqdbMysql.h', 'Sqdb-migration.h']
	dependencies += mysql
endif

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
               install_dir : 'include/sqxc',
               configuration : config_data)

sqxc_inc = include_directories('./')

sqxc_lib = static_library('sqxc',
                          sources,
                          include_directories: sqxc_inc,
                          dependencies : dependencies,
                          install: false)

sqxcpp_lib = static_library('sqxcpp',
                            sources_cpp,
                            include_directories: sqxc_inc,
                            dependencies : dependencies,
                            install: false)

sqxconsole_lib = static_library('sqxconsole',
                                sources_console,
                                include_directories: sqxc_inc,
                                dependencies : dependencies,
                                install: false)

sqxctest_lib = static_library('sqxctest',
                              sources_test,
                              include_directories: sqxc_inc,
                              dependencies : dependencies,
                              install: false)

sqxc = declare_dependency(include_directories : sqxc_inc,
                          link_with : [sqxc_lib, sqxcpp_lib],
                          dependencies : dependencies)
