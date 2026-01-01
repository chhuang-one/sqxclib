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
# The difference is that it links STATIC 'sqxc' library.
sqapp_data_lib = static_library(
    'sqapp-data',
    sources_app_data,
    include_directories: [sqapp_data_inc, sqxcapp_inc],
    dependencies: sqxc,
    install: false
)

# --- 'sqapp-data-link_shared' library ---
# It has database settings and migration data for application.
# The difference is that it links SHARED 'sqxc' library.
sqapp_data_link_shared_lib = static_library(
    'sqapp-data-link-shared',
    sources_app_data,
    include_directories: [sqapp_data_inc, sqxcapp_inc],
    dependencies: sqxc_shared,
    install: false
)

# change static dependency
sqxcapp = declare_dependency(
    include_directories : [sqapp_data_inc, sqxcapp_inc],
    link_with: [sqapp_data_lib, sqxcapp_lib],
    dependencies: sqxc
)

sqxcapptool = declare_dependency(
    include_directories : [sqapp_data_inc, sqxcapp_inc],
    link_with: [sqapp_data_lib, sqxcapptool_lib, sqxcapp_lib],
    dependencies: sqxcsupport
)

sqxc = sqxcapptool

# change shared dependency
sqxcapp_shared = declare_dependency(
    include_directories : [sqapp_data_inc, sqxcapp_inc],
    link_with: [sqapp_data_link_shared_lib, sqxcapp_shared_lib],
    dependencies: sqxc_shared
)

sqxcapptool_shared = declare_dependency(
    include_directories : [sqapp_data_inc, sqxcapp_inc],
    link_with: [sqapp_data_link_shared_lib, sqxcapptool_shared_lib, sqxcapp_shared_lib],
    dependencies: sqxcsupport_shared
)

sqxc_shared = sqxcapptool_shared

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
