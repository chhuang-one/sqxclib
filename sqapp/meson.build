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
                'CommandMigrate.c',
]

headers_tool = ['SqApp.h',
                'SqAppTool.h',
                'migrations.h',
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

executable('sqtool',
           sources,
           link_with : [sqxconsole_lib, sqtool_lib],
           dependencies : sqxc)
