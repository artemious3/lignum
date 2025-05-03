# renderer_src =  ['ctrl/renderer/ancestors-node-placer.cpp',
# 'ctrl/renderer/cluster.cpp',
# 'ctrl/renderer/descendants-node-placer.cpp',
# 'ctrl/renderer/renderer.cpp',
# 'ctrl/renderer/render-preprocessor.cpp']

renderer_src =  ['ancestors-node-placer.cpp',
'cluster.cpp',
'descendants-node-placer.cpp',
'renderer.cpp',
'render-preprocessor.cpp']

renderer_inc = [include_directories('.')]


renderer_lib =  static_library('lignumRenderer',
                             renderer_src,
                             include_directories : renderer_inc + model_inc + config_inc,
                             dependencies : [qt6_dep, spdlog_dep])
