# --- sqapp ---
sources_app = ['SqApp.c',
               'SqMigration.c',
               'migrations.c',
               'migrations-files.c'
]

sources_app_cpp = ['migrations-files.cpp']

headers_app = ['SqApp.h',
               'SqApp-config.h',
               'SqMigration.h',
               'migrations.h',
]

# --- sqapptool ---
sources_apptool = ['SqApp.c',
                   'SqPairs.c',
                   'SqAppTool.c',
                   'SqAppTool-migrations-files.c',
                   'SqMigration.c',
                   'migrations.c',
                   'CommonOptions.c',
                   'CommandMigrate.c',
                   'CommandMake.c',
]

sources_apptool_cpp = ['SqAppTool-migrations-files.cpp']

headers_apptool = ['SqApp.h',
                   'SqApp-config.h',
                   'SqPairs.h',
                   'SqAppTool.h',
                   'SqAppTool-config.h',
                   'SqMigration.h',
                   'migrations.h',
                   'CommonOptions.h',
                   'CommandMigrate.h',
                   'CommandMake.h',
]

# --- sqtool ---
sources = ['sqtool.c']

sources_cpp = ['sqtoolcpp.cpp']

# ---
sqapp_inc = include_directories('./')

sqapp_lib = static_library('sqapp',
                           sources_app,
                           include_directories: sqapp_inc,
                           dependencies : sqxc,
                           install: false)

sqappcpp_lib = static_library('sqappcpp',
                              sources_app_cpp,
                              include_directories: sqapp_inc,
                              dependencies : sqxc,
                              install: false)

sqapptool_inc = sqapp_inc

sqapptool_lib = static_library('sqapptool',
                               sources_apptool,
                               include_directories: sqapp_inc,
                               dependencies : sqxc,
                               install: false)

sqapptoolcpp_lib = static_library('sqapptoolcpp',
                                  sources_apptool_cpp,
                                  include_directories: sqapp_inc,
                                  dependencies : sqxc,
                                  install: false)

sqapp = declare_dependency(include_directories : sqapp_inc,
                           link_with : [sqapp_lib, sqappcpp_lib],
                           dependencies : sqxc)

sqapptool = declare_dependency(include_directories : sqapp_inc,
                               link_with : [sqapptool_lib, sqapptoolcpp_lib, sqxconsole_lib],
                               dependencies : sqxc)

executable('sqtool',
           sources,
           dependencies : sqapptool)

executable('sqtoolcpp',
           sources_cpp,
           dependencies : sqapptool)
