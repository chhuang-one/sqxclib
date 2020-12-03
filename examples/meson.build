
executable('storage-c99',
           'storage-c99.c',
           include_directories : sqxc_inc,
		   dependencies : [sqlite, jsonc],
           link_with : sqxc)

executable('storage-cxx',
           'storage-cxx.cpp',
           include_directories : sqxc_inc,
		   dependencies : [sqlite, jsonc],
           link_with : [sqxc, sqxcpp])
