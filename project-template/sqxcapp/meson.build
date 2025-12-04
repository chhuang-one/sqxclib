# --- 'sqxcapp-data' sources and headers ---
sources_app_data = [
    'SqApp-config.c',
    'migrations.c',
    'migrations-files.c',
    'migrations-files-cxx.cpp',
]

headers_app_data = [
    'SqApp-config.h',
    'migrations.h',
]

# --- 'sqtool' sources ---
sources = ['sqtool.c']

sources_cpp = ['sqtool-cpp.cpp']

# ---

sqxcapp_inc = include_directories('./')
sqxcapptool_inc = sqxcapp_inc

# --- 'sqxcapp-data' library ---
# 'sqxcapp-data' has database settings and migration data for user application.
sqxcapp_data_lib = static_library(
    'sqxcapp-data',
    sources_app_data,
    include_directories: sqxcapp_inc,
    dependencies : sqxc,
    install: false
)

sqxcapp = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with : sqxcapp_data_lib,
    dependencies : sqxc
)

sqxcapptool = declare_dependency(
    include_directories : sqxcapp_inc,
    link_with : sqxcapp_data_lib,
    dependencies : sqxc
)

# --- 'sqtool' executable file ---
executable('sqtool',
           sources,
           dependencies : sqxcapptool)

# --- 'sqtool-cpp' executable file ---
executable('sqtool-cpp',
           sources_cpp,
           dependencies : sqxcapptool)
