compiler = meson.get_compiler('c')

# bili2mp4 can't compile with MSVC because it uses readdir()...etc functions.
if compiler.get_id() != 'msvc'
	if jsonc.found() == true
		executable('bili2mp4',
		           'bili2mp4.c',
		           dependencies : [sqxc])
	endif
endif

executable('storage-c99',
           'storage-c99.c',
           dependencies : [sqxc])

executable('storage-cxx',
           'storage-cxx.cpp',
           dependencies : [sqxc])

executable('storage-row',
           'storage-row.cpp',
           dependencies : [sqxcsupport])

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
	           dependencies : [sqxcsupport])
endif
