compiler = meson.get_compiler('c')

# bili2mp4
# --- cJSON or json-c is required
if cjson.found()  or  jsonc.found()
	# If you want to build bili2mp4 in Visual Studio, download direct.h for MSVC form internet.
	if compiler.get_id() != 'msvc'
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

# --- cJSON or json-c is required
if cjson.found()  or  jsonc.found()
	executable('xc_json_file',
	           'xc_json_file.cpp',
	           dependencies : [sqxcsupport])
endif
