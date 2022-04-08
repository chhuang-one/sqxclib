
executable('test-container-cxx',
           'test-container-cxx.cpp',
           dependencies : sqxc)

executable('test-cxx',
           'test-cxx.cpp',
           dependencies : sqxcsupport,
           link_with: sqxctest_lib)

executable('test-migration',
           'test-migration.c',
           dependencies : sqxc)

executable('test-query',
           'test-query.c',
           dependencies : sqxc)

executable('test-sqxc',
           'test-sqxc.c',
           dependencies : sqxcsupport,
           link_with: sqxctest_lib)

executable('test-storage',
           'test-storage.c',
           dependencies : sqxc)

executable('test-util',
           'test-util.c',
           dependencies : sqxc)

