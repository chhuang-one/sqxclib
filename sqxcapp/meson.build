# --- 'sqxcapp' sources and headers ---
sources_app = [
    'SqApp.c',
    'SqMigration.c',
]

headers_app = [
    'SqApp.h',
    'SqMigration.h',
]

sources_app_data = [
    'SqApp-config.c',
    'migrations.c',
    'migrations-files.c',
    'migrations-files-cpp.cpp',
]

headers_app_data = [
    'SqApp-config.h',
    'migrations.h',
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

# --- 'sqtool' sources ---
sources = ['sqtool.c']

sources_cpp = ['sqtool-cpp.cpp']

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

# 'sqxcapp-data' has database settings and migration data for user application.
# The difference is that it links STATIC 'sqxc' library.
sqxcapp_data_lib = static_library(
    'sqxcapp-data',
    sources_app_data,
    include_directories: sqxcapp_inc,
    dependencies: sqxc,
    install: false
)

# 'sqxcapp-data-link_shared' has database settings and migration data for user application.
# The difference is that it links SHARED 'sqxc' library.
sqxcapp_data_link_shared_lib = static_library(
    'sqxcapp-data-link-shared',
    sources_app_data,
    include_directories: sqxcapp_inc,
    dependencies: sqxc_shared,
    install: false
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
    link_with: [sqxcapp_data_lib, sqxcapp_lib],
    dependencies: sqxc
)

sqxcapptool = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with: [sqxcapp_data_lib, sqxcapptool_lib, sqxcapp_lib],
    dependencies: sqxcsupport
)

# shared dependency
sqxcapp_shared = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with: [sqxcapp_data_link_shared_lib, sqxcapp_shared_lib],
    dependencies: sqxc_shared
)

sqxcapptool_shared = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with: [sqxcapp_data_link_shared_lib, sqxcapptool_shared_lib, sqxcapp_shared_lib],
    dependencies: sqxcsupport_shared
)

# --- 'sqtool' executable file ---
executable('sqtool',
           sources,
           dependencies: sqxcapptool)

# --- 'sqtool-cpp' executable file ---
executable('sqtool-cpp',
           sources_cpp,
           dependencies: sqxcapptool)
