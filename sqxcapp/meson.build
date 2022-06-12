# --- sqxcapp ---
sources_app = [
    'SqApp.c',
    'SqMigration.c',
]

headers_app = [
    'SqApp.h',
    'SqMigration.h',
]

sources_app_user = [
    'SqApp-config.c',
    'migrations.c',
    'migrations-files.c',
    'migrations-files.cpp',
]

headers_app_user = [
    'SqApp-config.h',
    'migrations.h',
]

# --- sqxcapptool ---
sources_apptool = [
    'SqAppTool.c',
    'CommandCommon.c',
    'CommandMigrate.c',
    'CommandMake.c',
]

headers_apptool = [
    'SqAppTool.h',
    'SqAppTool-config.h',
    'CommandCommon.h',
    'CommandMigrate.h',
    'CommandMake.h',
]

sources_apptool_user = [
    'SqApp-config.c',
    'migrations.c',
    'SqAppTool-migrations-files.c',
    'SqAppTool-migrations-files.cpp'
]

# --- sqxctool ---
sources = ['sqxctool.c']

sources_cpp = ['sqxcpptool.cpp']

# ---

install_headers(headers_app, subdir: 'sqxc/app')
install_headers(headers_apptool, subdir: 'sqxc/app')

sqxcapp_inc = include_directories('./')
sqxcapptool_inc = sqxcapp_inc

# 'sqxcapp' library
sqxcapp_lib = static_library(
    'sqxcapp',
    sources_app,
    include_directories: sqxcapp_inc,
    dependencies : sqxc,
    install: true
)

sqxcapp_shared_lib = shared_library(
    'sqxcapp',
    sources_app,
    include_directories: sqxcapp_inc,
    dependencies : sqxc_shared,
    install: true
)

# 'sqxcapp-user' has database settings and migration data for user application.
sqxcapp_user_lib = static_library(
    'sqxcapp-user',
    sources_app_user,
    include_directories: sqxcapp_inc,
    dependencies : sqxc,
    install: false
)

sqxcapp_user_lib_link_shared = static_library(
    'sqxcapp-user-link-shared',
    sources_app_user,
    include_directories: sqxcapp_inc,
    dependencies : sqxc_shared,
    install: false
)

# 'sqxcapptool' library
sqxcapptool_lib = static_library(
    'sqxcapptool',
    sources_apptool,
    include_directories: sqxcapp_inc,
    dependencies : sqxcsupport,
    install: true
)

sqxcapptool_shared_lib = shared_library(
    'sqxcapptool',
    sources_apptool,
    include_directories: sqxcapp_inc,
    link_with : sqxcapp_shared_lib,
    dependencies : sqxcsupport_shared,
    install: true
)

# 'sqxcapptool-user' has database settings and migration data for user application.
sqxcapptool_user_lib = static_library(
    'sqxcapptool-user',
    sources_apptool_user,
    include_directories: sqxcapp_inc,
    dependencies : sqxcsupport,
    install: false
)

sqxcapptool_user_lib_link_shared = static_library(
    'sqxcapptool-user-link-shared',
    sources_apptool_user,
    include_directories: sqxcapp_inc,
    dependencies : sqxcsupport_shared,
    install: false
)

sqxcapp = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with : [sqxcapp_user_lib, sqxcapp_lib],
    dependencies : sqxc
)

sqxcapp_shared = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with : [sqxcapp_user_lib_link_shared, sqxcapp_shared_lib],
    dependencies : sqxc_shared
)

sqxcapptool = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with : [sqxcapptool_user_lib, sqxcapptool_lib, sqxcapp_lib],
    dependencies : sqxcsupport
)

sqxcapptool_shared = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with : [sqxcapptool_user_lib_link_shared, sqxcapptool_shared_lib, sqxcapp_shared_lib],
    dependencies : sqxcsupport_shared
)

executable('sqxctool',
           sources,
           dependencies : sqxcapptool)

executable('sqxcpptool',
           sources_cpp,
           dependencies : sqxcapptool)
