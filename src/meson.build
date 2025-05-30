
subdir('cfg')

subdir('model')


view_inc = [include_directories('view'), include_directories('view/ft')]
control_inc = [include_directories('ctrl'), include_directories('ctrl/renderer')]


subdir('ctrl')
subdir('view')



lignum = executable('lignum', 
                    ['main.cpp'],
                     # include_directories :['cfg', 'ctrl', 'view', 'view/ft', 'view/ft/abstract', 'view/ft/impl','.', 'ctrl/renderer', 'model'],
                     include_directories : view_inc + model_inc +  control_inc,
                     dependencies : [qt6_dep, spdlog_dep],
                     link_with : [model_dep, control_lib,  view_lib, config_lib, renderer_lib, gedcom_loader],
                     cpp_args : compiler_flags,
                    link_args : ['-lmongocxx', '-lbsoncxx']
                     )
