# helpers sources & headers
sources_helpers = [
    'SqStr.c',
]

headers_helpers = [
    'SqStr.h',
]

# data structs sources & headers
sources_structs = [
    'SqPairs.c',
    'SqRow.c',
#    'SqCollection.c',
]

headers_structs = [
    'SqPairs.h',
    'SqRow.h',
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

# --- sqxcsupport ---
headers_support = [
    headers_helpers,
    headers_structs,
    headers_console,
    headers_test,
]

install_headers(headers_support, subdir: 'sqxc')

sqxcsupport_inc = include_directories('./')

sqxcsupport_lib = static_library('sqxcsupport',
                                 [sources_helpers, sources_structs, sources_console, sources_test],
                                 include_directories: sqxcsupport_inc,
                                 dependencies : sqxc,
                                 install: false)

sqxcsupport = declare_dependency(include_directories : sqxcsupport_inc,
                                 link_with : sqxcsupport_lib,
                                 dependencies : sqxc)
