# --- 'sqxcapp-user' sources and headers ---
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

# --- 'sqxcapptool-user' sources and headers ---
sources_apptool_user = [
    'SqApp-config.c',
    'migrations.c',
    'SqAppTool-migrations-files.c',
    'SqAppTool-migrations-files.cpp'
]

headers_apptool_user = [
    'SqAppTool-config.h',
]

# --- 'sqtool' sources ---
sources = ['sqtool.c']

sources_cpp = ['sqtool-cpp.cpp']

# ---

sqxcapp_inc = include_directories('./')
sqxcapptool_inc = sqxcapp_inc

# --- 'sqxcapp-user' library ---
# 'sqxcapp-user' has database settings and migration data for user application.
sqxcapp_user_lib = static_library(
    'sqxcapp-user',
    sources_app_user,
    include_directories: sqxcapp_inc,
    dependencies : sqxc,
    install: false
)

# --- 'sqxcapptool-user' library ---
# 'sqxcapptool-user' has database settings and migration data for user application.
sqxcapptool_user_lib = static_library(
    'sqxcapptool-user',
    sources_apptool_user,
    include_directories: sqxcapp_inc,
    dependencies : sqxc,
    install: false
)

sqxcapp = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with : sqxcapp_user_lib,
    dependencies : sqxc
)

sqxcapptool = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with : sqxcapptool_user_lib,
    dependencies : sqxc
)

# --- 'sqtool' executable file ---
executable('sqtool',
           sources,
           dependencies : sqxcapptool)

# --- deprecated 'sqxctool' executable file ---
executable('sqxctool',
           sources,
           dependencies : sqxcapptool)

# --- 'sqtool-cpp' executable file ---
executable('sqtool-cpp',
           sources_cpp,
           dependencies : sqxcapptool)

# --- deprecated 'sqxcpptool' executable file ---
executable('sqxcpptool',
           sources_cpp,
           dependencies : sqxcapptool)
