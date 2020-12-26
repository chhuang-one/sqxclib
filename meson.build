project('sqxc', 'c', 'cpp',
#        default_options : ['c_std=c99', 'cpp_std=c++11'],
        version: '0.0.6')

add_global_arguments('-DDEBUG', language : ['c', 'cpp'])

jsonc = dependency('json-c', required: true)
sqlite = dependency('sqlite3', required: true)

subdir('sqxc')
subdir('tests')
subdir('examples')

