
executable('test-container-cxx',
           'test-container-cxx.cpp',
           include_directories : sqxc_inc,
           link_with : sqxc)

executable('test-cxx',
           'test-cxx.cpp',
           include_directories : sqxc_inc,
		   dependencies : sqlite,
           link_with : sqxc)

executable('test-migration',
           'test-migration.c',
           include_directories : sqxc_inc,
		   dependencies : sqlite,
           link_with : sqxc)

executable('test-query',
           'test-query.c',
           include_directories : sqxc_inc,
		   dependencies : sqlite,
           link_with : sqxc)

executable('test-sqxc',
           'test-sqxc.c',
           include_directories : sqxc_inc,
		   dependencies : [sqlite, jsonc],
           link_with : sqxc)

executable('test-storage',
           'test-storage.c',
           include_directories : sqxc_inc,
		   dependencies : [sqlite, jsonc],
           link_with : sqxc)

executable('test-util',
           'test-util.c',
           include_directories : sqxc_inc,
           link_with : sqxc)

