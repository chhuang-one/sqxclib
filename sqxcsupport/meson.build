# helpers sources & headers
sources_helpers = [
    'SqHelpers.c',
]

headers_helpers = [
    'SqHelpers.h',
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
    'SqxcMem.c',
]

headers_test = [
    'SqdbEmpty.h',
    'SqxcEmpty.h',
    'SqxcFile.h',
    'SqxcMem.h',
]

# --- sqxcsupport ---
headers_support = [
    headers_helpers,
    headers_structs,
    headers_console,
    headers_test,
    'sqxcsupport.h',
]

install_headers(headers_support, subdir: 'sqxc/support')

sqxcsupport_inc = include_directories('./')

sqxcsupport_lib = static_library(
    'sqxcsupport',
    [sources_helpers, sources_structs, sources_console, sources_test],
    include_directories: [sqxcsupport_inc, sqxc_inc],
#    dependencies: sqxc,
    install: true
)

sqxcsupport_shared_lib = shared_library(
    'sqxcsupport',
    [sources_helpers, sources_structs, sources_console, sources_test],
    include_directories: [sqxcsupport_inc, sqxc_inc],
    link_with: sqxc_shared_lib,
#    dependencies: sqxc_shared,
    version: sqxclib_version,
#    soversion: sqxclib_soversion,
    install: true
)

# static dependency
sqxcsupport = declare_dependency(
    include_directories: sqxcsupport_inc,
    link_with: sqxcsupport_lib,
    dependencies: sqxc
)

# shared dependency
sqxcsupport_shared = declare_dependency(
    include_directories: sqxcsupport_inc,
    link_with: sqxcsupport_shared_lib,
    dependencies: sqxc_shared
)
