


# control_src = ['ctrl/family-tree-builder.cpp', 'ctrl/tree-manager.cpp']
control_src = ['tree-manager.cpp']

subdir('renderer')


control_lib = static_library('lignumControl',
                            control_src,
                            include_directories : model_inc + control_inc + view_inc,
                            dependencies : [qt6_dep, spdlog_dep])
