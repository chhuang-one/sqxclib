sources_helpers = [
    'SqStr.c',
]

headers_helpers = [
    'SqStr.h',
]

sources_structs = [
    'SqPairs.c',
#    'SqCollection.c',
]

headers_structs = [
    'SqPairs.h',
#    'SqCollection.h',
]

# console sources & headers

sources_console = [
    'SqConsole.c',
    'SqCommand.c',
    'SqOption.c',
]

headers_console = [
    'SqConsole.h',
    'SqCommand.h',
    'SqOption.h',
]

# test sources & headers

sources_test = [
    'SqdbEmpty.c',
    'SqxcEmpty.c',
    'SqxcFile.c',
]

headers_test = [
    'SqdbEmpty.h',
    'SqxcEmpty.h',
    'SqxcFile.h',
]

install_headers(headers_helpers, subdir: 'sqxc')
install_headers(headers_structs, subdir: 'sqxc')
install_headers(headers_console, subdir: 'sqxc')
install_headers(headers_test,    subdir: 'sqxc')

sqxcsupport_inc = include_directories('./')

sqxchelpers_lib = static_library('sqxchelpers',
                                 sources_helpers,
                                 include_directories: sqxcsupport_inc,
                                 dependencies : sqxc,
                                 install: false)

sqxcstructs_lib = static_library('sqxcstructs',
                                 sources_structs,
                                 include_directories: sqxcsupport_inc,
                                 dependencies : sqxc,
                                 install: false)

sqxconsole_lib = static_library('sqxconsole',
                                sources_console,
                                include_directories: sqxcsupport_inc,
                                dependencies : sqxc,
                                install: false)

sqxctest_lib = static_library('sqxctest',
                              sources_test,
                              include_directories: sqxcsupport_inc,
                              dependencies : sqxc,
                              install: false)

sqxcsupport = declare_dependency(include_directories : sqxcsupport_inc,
                                 link_with : [sqxchelpers_lib, sqxcstructs_lib, sqxconsole_lib, sqxctest_lib],
                                 dependencies : sqxc)
