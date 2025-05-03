
# view_src = ['view/family-tree-graphics-view.cpp',
#             'view/lignum-window.cpp',
#             'view/person-view.cpp',
#             'view/family-tree-view.cpp']

view_src = ['family-tree-graphics-view.cpp',
            'lignum-window.cpp',
            'person-view.cpp',
            'family-tree-view.cpp']

view_src += qt6.compile_moc(headers : ['lignum-window.h',
                                       'person-view.h',
                                       'family-tree-graphics-view.h',
                                       'family-tree-view.h'])

view_src += qt6.compile_ui(sources : 'lignum-window.ui')


subdir('ft')
view_src += family_tree_src

subdir('res')
view_src += res_src


view_lib = static_library('lignumView', 
                        view_src,
                        include_directories : model_inc + view_inc + config_inc + control_inc,
                        dependencies : [qt6_dep, spdlog_dep])
