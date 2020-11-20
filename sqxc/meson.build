sources = ['SqPtrArray.c',
           'SqBuffer.c',
           'SqUtil.c',
           'SqType.c',
           'SqType-built-in.c',
           'SqType-PtrArray.c',
           'SqEntry.c',
           'SqTable.c',
           'SqJoint.c',
           'SqSchema.c',
           'SqStorage.c',
           'Sqdb.c',
           'SqdbSqlite.c',
           'Sqxc.c',
           'SqxcValue.c',
           'SqxcJsonc.c',
           'SqxcSql.c',
           'SqQuery.c',

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
           'Sqdb.h',
           'SqdbSqlite.h',
           'Sqxc.h',
           'SqxcValue.h',
           'SqxcJsonc.h',
           'SqxcSql.h',
           'SqQuery.h', 'SqQuery-macro.h',

           'SqConfig.h',

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

# install_headers(headers, subdir : 'sqxc')
# install_headers(headers_cpp, subdir : 'sqxc')

sqxc = static_library('sqxc',
                      sources,
                      include_directories: sqxc_inc,
                      install: true)

sqxcpp = static_library('sqxcpp',
                        sources_cpp,
                        include_directories: sqxc_inc,
                        install: true)

#                     dependencies : [json-c, sqlite3],


# sqxc_dep = declare_dependency(
#		include_directories : sqxc_inc
#		link_with : sqxc)
#
