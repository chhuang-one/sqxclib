
executable('storage-c99',
           'storage-c99.c',
           dependencies : [sqxc])

executable('storage-cxx',
           'storage-cxx.cpp',
           dependencies : [sqxc])

executable('myapp-c99',
           'myapp-c99.c',
           dependencies : [sqxcapp])

executable('myapp-cxx',
           'myapp-cxx.cpp',
           dependencies : [sqxcapp])

# --- json-c is required
if jsonc.found() == true
executable('xc_json_file',
           'xc_json_file.cpp',
           dependencies : [sqxcsupport],
           link_with: sqxctest_lib)
endif
