
subdir('cfg')
subdir('model')
subdir('ctrl')
subdir('view')


lignum = executable('lignum', 
                    ['main.cpp'] + config_src + view_src + control_src,
                     include_directories :['cfg', 'ctrl', 'view', 'view/ft', 'view/ft/abstract', 'view/ft/impl','.', 'ctrl/renderer', 'model'],
                     dependencies : [qt6_dep, spdlog_dep],
                     link_with : model_dep,
                     cpp_args : '-Wno-changes-meaning')
