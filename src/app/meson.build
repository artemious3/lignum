subdir('config')
subdir('family-tree')
subdir('main-window')
subdir('renderer')
subdir('res')


config_inc = include_directories('config')
ftree_inc = include_directories('family-tree')
ftree_default_inc = include_directories('family-tree/default-theme')

lignum = executable('lignum', 
                     ['main.cpp'] + config_src + family_tree_src + main_window_src + renderer_src + res_src,
                     include_directories :['config', 'family-tree','family-tree/default-theme', 'main-window', 'renderer', '../model/'],
                     dependencies : [qt6_dep, spdlog_dep],
                     link_with : db_dep,
                     cpp_args : '-Wno-changes-meaning')
