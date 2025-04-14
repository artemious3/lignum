family_tree_src = ['family-tree/family-tree-builder.cpp',
'family-tree/family-tree-item.cpp',
'family-tree/people-connector-builder.cpp',
'family-tree/render-manager.cpp',]

default_theme_src = ['family-tree/default-theme/connector.cpp',
'family-tree/default-theme/family-connector.cpp',
'family-tree/default-theme/person-item.cpp']

family_tree_src += default_theme_src

family_tree_src += qt6.compile_moc(headers : ['family-tree-item.h', 'default-theme/person-item.h'])


