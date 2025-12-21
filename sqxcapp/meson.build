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

# --- build 'sqxcapp-data' & 'sqtool' ---
# 'sqxcapp-data' include dir
sqxcapp_data_inc = include_directories('./')

# --- 'sqxcapp-data' library ---
# It has database settings and migration data for user application.
# The difference is that it links STATIC 'sqxc' library.
sqxcapp_data_lib = static_library(
    'sqxcapp-data',
    sources_app_data,
    include_directories: [sqxcapp_data_inc, sqxcapp_inc],
    dependencies: sqxc,
    install: false
)

# --- 'sqxcapp-data-link_shared' library ---
# It has database settings and migration data for user application.
# The difference is that it links SHARED 'sqxc' library.
sqxcapp_data_link_shared_lib = static_library(
    'sqxcapp-data-link-shared',
    sources_app_data,
    include_directories: [sqxcapp_data_inc, sqxcapp_inc],
    dependencies: sqxc_shared,
    install: false
)

# change static dependency
sqxcapp = declare_dependency(
    include_directories : [sqxcapp_data_inc, sqxcapp_inc],
    link_with: [sqxcapp_data_lib, sqxcapp_lib],
    dependencies: sqxc
)

sqxcapptool = declare_dependency(
    include_directories : [sqxcapp_data_inc, sqxcapp_inc],
    link_with: [sqxcapp_data_lib, sqxcapptool_lib, sqxcapp_lib],
    dependencies: sqxcsupport
)

# change shared dependency
sqxcapp_shared = declare_dependency(
    include_directories : [sqxcapp_data_inc, sqxcapp_inc],
    link_with: [sqxcapp_data_link_shared_lib, sqxcapp_shared_lib],
    dependencies: sqxc_shared
)

sqxcapptool_shared = declare_dependency(
    include_directories : [sqxcapp_data_inc, sqxcapp_inc],
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
