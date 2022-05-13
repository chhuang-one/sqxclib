# --- sqxcapp ---
sources_app = [
    'SqApp.c',
    'SqMigration.c',
    'migrations.c',
    'migrations-files.c'
]

sources_app_cpp = [
    'migrations-files.cpp'
]

headers_app = [
    'SqApp.h',
    'SqApp-config.h',
    'SqMigration.h',
    'migrations.h',
]

# --- sqxcapptool ---
sources_apptool = [
    'SqApp.c',
    'SqAppTool.c',
    'SqAppTool-migrations-files.c',
    'SqMigration.c',
    'migrations.c',
    'CommandCommon.c',
    'CommandMigrate.c',
    'CommandMake.c',
]

sources_apptool_cpp = [
    'SqAppTool-migrations-files.cpp'
]

headers_apptool = [
    'SqApp.h',
    'SqApp-config.h',
    'SqAppTool.h',
    'SqAppTool-config.h',
    'SqMigration.h',
    'migrations.h',
    'CommandCommon.h',
    'CommandMigrate.h',
    'CommandMake.h',
]

# --- sqxctool ---
sources = ['sqxctool.c']

sources_cpp = ['sqxcpptool.cpp']

# ---
sqxcapp_inc = include_directories('./')

sqxcapp_lib = static_library('sqxcapp',
                             sources_app,
                             include_directories: sqxcapp_inc,
                             dependencies : sqxc,
                             install: false)

sqxcppapp_lib = static_library('sqxcppapp',
                               sources_app_cpp,
                               include_directories: sqxcapp_inc,
                               dependencies : sqxc,
                               install: false)

sqxcapptool_inc = sqxcapp_inc

sqxcapptool_lib = static_library('sqxcapptool',
                                 sources_apptool,
                                 include_directories: sqxcapp_inc,
                                 dependencies : sqxcsupport,
                                 install: false)

sqxcppapptool_lib = static_library('sqxcppapptool',
                                   sources_apptool_cpp,
                                   include_directories: sqxcapp_inc,
                                   dependencies : sqxcsupport,
                                   install: false)

sqxcapp = declare_dependency(include_directories : sqxcapp_inc,
                             link_with : [sqxcapp_lib, sqxcppapp_lib],
                             dependencies : sqxc)

sqxcapptool = declare_dependency(include_directories : sqxcapp_inc,
                                 link_with : [sqxcapptool_lib, sqxcppapptool_lib],
                                 dependencies : sqxcsupport)

executable('sqxctool',
           sources,
           dependencies : sqxcapptool)

executable('sqxcpptool',
           sources_cpp,
           dependencies : sqxcapptool)
