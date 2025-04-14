subdir('config')
subdir('family-tree')
subdir('main-window')
subdir('renderer')
subdir('res')

lignum = executable('lignum', 
                     ['main.cpp'] + config_src + family_tree_src + main_window_src + renderer_src + res_src,
                     include_directories :['config', 'family-tree','family-tree/default-theme', 'main-window', 'renderer', '../database/'],
                     dependencies : [qt6_dep, spdlog_dep],
                     link_with : db_dep,
                     cpp_args : '-Wno-changes-meaning')
