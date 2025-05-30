


gtest_dep = dependency('gtest')


sql_test = executable('sql_tst',
                 ['database/basic_sql_test.cpp'],
                 link_with : model_dep,
                 include_directories : [model_inc, config_inc, view_inc, control_inc],
                 dependencies : [qt6_dep, gtest_dep])


bio_test = executable('bio_test',
           ['biographies-model-test.cpp'],
            cpp_args: [ '-I/usr/include/mongocxx/v_noabi/', '-I/usr/include/bsoncxx/v_noabi/'],
            link_args: ['-lmongocxx', '-lbsoncxx'],
               include_directories : [model_inc, config_inc, view_inc, control_inc],
               link_with : biographies_lib,
            dependencies:[gtest_dep])

