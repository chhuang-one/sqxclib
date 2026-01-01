# --- 'sqapp-data' sources and headers ---
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

sources_cxx = ['sqtool-cxx.cpp']

# --- build 'sqapp-data' & 'sqtool' ---
# 'sqapp-data' include dir
sqapp_data_inc = include_directories('./')

# --- 'sqapp-data' library ---
# It has database settings and migration data for application.
sqapp_data_lib = static_library(
    'sqapp-data',
    sources_app_data,
    include_directories: sqapp_data_inc,
    dependencies: sqxc,
    install: false
)

# dependency
sqxcapp = declare_dependency(
    include_directories : sqapp_data_inc,
    link_with: sqapp_data_lib,
    dependencies: sqxc
)

sqxcapptool = declare_dependency(
    include_directories : sqapp_data_inc,
    link_with: sqapp_data_lib,
    dependencies: sqxc
)

# change dependency
sqxc = sqxcapptool

# --- 'sqtool' executable file ---
executable('sqtool',
           sources,
           dependencies: sqxcapptool)

# --- 'sqtool-cxx' executable file ---
executable('sqtool-cxx',
           sources_cxx,
           dependencies: sqxcapptool)

# deprecated
# --- 'sqtool-cpp' executable file ---
executable('sqtool-cpp',
           sources_cxx,
           dependencies: sqxcapptool)
