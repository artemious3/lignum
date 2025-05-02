


gtest_dep = dependency('gtest')


test = executable('tst',
                 ['database/basic_sql_test.cpp'],
                 link_with : db_dep,
                 include_directories : [db_inc, config_inc, ftree_inc, ftree_default_inc ],
                 dependencies : [qt6_dep, gtest_dep])
