model_inc=[include_directories('.')]

model_dep = static_library(
    'DB',
    sources : ['FamilyTreeSqlModel.cpp'],
    dependencies : [qt6_dep, spdlog_dep]
  )


gedcom_loader = static_library(
  'gedcom_loader',
  sources : ['GedcomLoader.cpp'],
  dependencies : [qt6_dep, gedcom_dep, spdlog_dep],
  link_with : [model_dep, config_lib],
  include_directories : [config_inc]
  )
