# link flags for gcc to avoid link with unused shared libraries
#gcc_link_flags = '-Wl,--as-needed'


# link shared library: 'sqxc'
executable('main',
           'main.c',
#           link_args : gcc_link_flags,
           dependencies : [sqxc])

# link shared library: 'sqxc' and 'sqxcapp'
executable('main-sqxcapp',
           'main-sqxcapp.c',
#           link_args : gcc_link_flags,
           dependencies : [sqxcapp])


# link shared library: 'sqxc'.
executable('main-cxx',
           'main-cxx.cpp',
#           link_args : gcc_link_flags,
           dependencies : [sqxc])

# link shared library: 'sqxc' and 'sqxcapp'
executable('main-sqxcapp-cxx',
           'main-sqxcapp-cxx.cpp',
#           link_args : gcc_link_flags,
           dependencies : [sqxcapp])
