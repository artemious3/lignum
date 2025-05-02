


gtest_dep = dependency('gtest')


sql_test = executable('sql_tst',
                 ['database/basic_sql_test.cpp'],
                 link_with : model_dep,
                 include_directories : [model_inc, config_inc, view_inc, control_inc],
                 dependencies : [qt6_dep, gtest_dep])

test('SQL test', sql_test)
