# --- 'sqxcapp' sources and headers ---
sources_app = [
    'SqApp.c',
    'SqMigration.c',
]

headers_app = [
    'sqxcapp.h',
    'SqApp.h',
    'SqMigration.h',
]

# --- 'sqxcapptool' sources and headers ---
sources_apptool = [
    'SqAppTool.c',
    'SqAppOptions.c',
    'SqAppCommands-migrate.c',
    'SqAppCommands-make.c',
]

headers_apptool = [
    'SqAppTool.h',
    'SqAppOptions.h',
    'SqAppCommands.h',
]

# ---

install_headers(headers_app, subdir: 'sqxc/app')
install_headers(headers_apptool, subdir: 'sqxc/app')

sqxcapp_inc = include_directories('./')
sqxcapptool_inc = sqxcapp_inc

# --- 'sqxcapp' library ---
sqxcapp_lib = static_library(
    'sqxcapp',
    sources_app,
    include_directories: [sqxcapp_inc, sqxc_inc],
#    dependencies: sqxc,
    install: true
)

sqxcapp_shared_lib = shared_library(
    'sqxcapp',
    sources_app,
    include_directories: [sqxcapp_inc, sqxc_inc],
    link_with: sqxc_shared_lib,
#    dependencies: sqxc_shared,
    version: sqxclib_version,
#    soversion: sqxclib_soversion,
    install: true
)

# --- 'sqxcapptool' library ---
sqxcapptool_lib = static_library(
    'sqxcapptool',
    sources_apptool,
    include_directories: [sqxcapp_inc, sqxcsupport_inc, sqxc_inc],
#    dependencies: sqxcsupport,
    install: true
)

sqxcapptool_shared_lib = shared_library(
    'sqxcapptool',
    sources_apptool,
    include_directories: [sqxcapp_inc, sqxcsupport_inc, sqxc_inc],
    link_with: [sqxcapp_shared_lib, sqxcsupport_shared_lib, sqxc_shared_lib],
#    dependencies: sqxcsupport_shared,
    version: sqxclib_version,
#    soversion: sqxclib_soversion,
    install: true
)

# static dependency
sqxcapp = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with: [sqxcapp_lib],
    dependencies: sqxc
)

sqxcapptool = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with: [sqxcapptool_lib, sqxcapp_lib],
    dependencies: sqxcsupport
)

# shared dependency
sqxcapp_shared = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with: [sqxcapp_shared_lib],
    dependencies: sqxc_shared
)

sqxcapptool_shared = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with: [sqxcapptool_shared_lib, sqxcapp_shared_lib],
    dependencies: sqxcsupport_shared
)
