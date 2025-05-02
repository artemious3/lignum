subdir('config')
subdir('family-tree')
subdir('renderer')
subdir('res')


config_inc = include_directories('config')
ftree_inc = include_directories('family-tree')
ftree_default_inc = include_directories('family-tree/default-theme')

app_root_src = ['main.cpp', 
               'family-tree-graphics-view.cpp',
               'lignum-window.cpp',
               'person-view.cpp',
               'tree-manager.cpp']

app_root_src += qt6.compile_moc(headers : ['lignum-window.h', 'person-view.h', 'family-tree-graphics-view.h'])
app_root_src += qt6.compile_ui(sources : 'lignum-window.ui')


lignum = executable('lignum', 
                    app_root_src + config_src + family_tree_src +  renderer_src + res_src,
                     include_directories :['config', 'family-tree','family-tree/default-theme', '.', 'renderer', '../model/'],
                     dependencies : [qt6_dep, spdlog_dep],
                     link_with : db_dep,
                     cpp_args : '-Wno-changes-meaning')
