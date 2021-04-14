sources = ['SqPtrArray.c',
           'SqBuffer.c',
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
           'SqRelation.c',

           # Sqdb - Database interface
           'Sqdb.c',

           # Sqxc - Converter interface
           'Sqxc.c',
           'SqxcValue.c',
           'SqxcJsonc.c',
           'SqxcSql.c',

           # for testing
           'SqdbEmpty.c',
           'SqxcEmpty.c',
          ]

headers = ['SqPtrArray.h',
           'SqBuffer.h',
           'SqDefine.h',
           'SqError.h',
           'SqUtil.h',
           'SqType.h',
           'SqEntry.h',
           'SqTable.h',
           'SqJoint.h',
           'SqSchema.h', 'SqSchema-macro.h',
           'SqStorage.h',
           'SqQuery.h', 'SqQuery-macro.h',
           'SqConfig.h',
           'SqRelation.h',

           # Sqdb - Database interface
           'Sqdb.h',

           # Sqxc - Converter interface
           'Sqxc.h',
           'SqxcValue.h',
           'SqxcJsonc.h',
           'SqxcSql.h',

           # for testing
           'SqdbEmpty.h',
           'SqxcEmpty.h',
          ]

# C++ sources & headers

sources_cpp = [
               'SqType-std-string.cpp',
              ]

headers_cpp = [
               'SqType-stl-cpp.h',
              ]

# install_headers(headers, subdir: 'sqxc')
# install_headers(headers_cpp, subdir: 'sqxc')

sqxc_inc = include_directories('./')
sqxc_dep = [jsonc]

if has_mysql == true
  sources += ['SqdbMysql.c', 'Sqdb-migration.c']
  headers += ['SqdbMysql.h', 'Sqdb-migration.h']
  sqxc_dep += mysql
endif

if has_sqlite == true
  sources += ['SqdbSqlite.c', 'SqRelation-migration.c']
  headers += ['SqdbSqlite.h', 'SqRelation-migration.h']
  sqxc_dep += sqlite
endif

sqxc_lib = static_library('sqxc',
                          sources,
                          include_directories: sqxc_inc,
                          dependencies : sqxc_dep,
                          install: false)

sqxcpp_lib = static_library('sqxcpp',
                            sources_cpp,
                            include_directories: sqxc_inc,
                            dependencies : sqxc_dep,
                            install: false)

sqxc = declare_dependency(include_directories : sqxc_inc,
                          link_with : [sqxc_lib, sqxcpp_lib])
