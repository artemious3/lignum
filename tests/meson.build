


gtest_dep = dependency('gtest')


test = executable('tst',
                 ['database/basic_sql_test.cpp'],
                 link_with : model_dep,
                 include_directories : [model_inc, config_inc],
                 dependencies : [qt6_dep, gtest_dep])
