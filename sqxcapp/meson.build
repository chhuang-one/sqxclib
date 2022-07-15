# --- 'sqxcapp' sources and headers ---
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

# --- 'sqxcapptool' sources and headers ---
sources_apptool = [
    'SqAppTool.c',
    'SqCommandCommon.c',
    'SqCommandMigrate.c',
    'SqCommandMake.c',
]

headers_apptool = [
    'SqAppTool.h',
    'SqCommandCommon.h',
    'SqCommandMigrate.h',
    'SqCommandMake.h',
]

sources_apptool_user = [
    'SqApp-config.c',
    'migrations.c',
    'SqAppTool-migrations-files.c',
    'SqAppTool-migrations-files.cpp'
]

headers_apptool_user = [
    'SqAppTool-config.h',
]

# --- 'sqxctool' sources ---
sources = ['sqxctool.c']

sources_cpp = ['sqxcpptool.cpp']

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

# 'sqxcapp-user' has database settings and migration data for user application.
# The difference is that it links STATIC 'sqxc' library.
sqxcapp_user_lib = static_library(
    'sqxcapp-user',
    sources_app_user,
    include_directories: sqxcapp_inc,
    dependencies: sqxc,
    install: false
)

# 'sqxcapp-user-link_shared' has database settings and migration data for user application.
# The difference is that it links SHARED 'sqxc' library.
sqxcapp_user_link_shared_lib = static_library(
    'sqxcapp-user-link-shared',
    sources_app_user,
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

# 'sqxcapptool-user' has database settings and migration data for user application.
# The difference is that it links STATIC 'sqxc' and 'sqxcsupport' library.
sqxcapptool_user_lib = static_library(
    'sqxcapptool-user',
    sources_apptool_user,
    include_directories: sqxcapp_inc,
    dependencies: sqxcsupport,
    install: false
)

# 'sqxcapptool-user-link-shared' has database settings and migration data for user application.
# The difference is that it links SHARED 'sqxc' and 'sqxcsupport' library.
sqxcapptool_user_link_shared_lib = static_library(
    'sqxcapptool-user-link-shared',
    sources_apptool_user,
    include_directories: sqxcapp_inc,
    dependencies: sqxcsupport_shared,
    install: false
)

# static dependency
sqxcapp = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with: [sqxcapp_user_lib, sqxcapp_lib],
    dependencies: sqxc
)

sqxcapptool = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with: [sqxcapptool_user_lib, sqxcapptool_lib, sqxcapp_lib],
    dependencies: sqxcsupport
)

# shared dependency
sqxcapp_shared = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with: [sqxcapp_user_link_shared_lib, sqxcapp_shared_lib],
    dependencies: sqxc_shared
)

sqxcapptool_shared = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with: [sqxcapptool_user_link_shared_lib, sqxcapptool_shared_lib, sqxcapp_shared_lib],
    dependencies: sqxcsupport_shared
)

# --- 'sqxctool' executable file ---
executable('sqxctool',
           sources,
           dependencies: sqxcapptool)

# --- 'sqxcpptool' executable file ---
executable('sqxcpptool',
           sources_cpp,
           dependencies: sqxcapptool)
