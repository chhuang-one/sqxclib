# --- sqapp ---
sources_app = ['SqApp.c',
               'migrations.c',
]

headers_app = ['SqApp.h',
               'migrations.h',
]

# --- sqapptool ---
sources_tool = ['SqApp.c',
                'SqAppTool.c',
                'SqAppTool-migrations.c',
                'CommonOptions.c',
                'CommandMigrate.c',
]

headers_tool = ['SqApp.h',
                'SqAppTool.h',
                'migrations.h',
                'CommonOptions.h',
                'CommandMigrate.h',
]

# --- sqtool ---
sources = ['sqtool.c']

# ---
sqapp_inc = include_directories('./')

sqapp_lib = static_library('sqapp',
                           sources_app,
                           include_directories: sqapp_inc,
                           dependencies : sqxc,
                           install: false)

sqtool_lib = static_library('sqapptool',
                            sources_tool,
                            include_directories: sqapp_inc,
                            dependencies : sqxc,
                            install: false)

sqapp = declare_dependency(include_directories : sqapp_inc,
                           link_with : sqapp_lib,
                           dependencies : sqxc)

sqtool = declare_dependency(include_directories : sqapp_inc,
                            link_with : [sqtool_lib, sqxconsole_lib],
                            dependencies : sqxc)

executable('sqtool',
           sources,
           dependencies : sqtool)
