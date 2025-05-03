
project('lignum', 'cpp', 
  default_options : ['cpp_std=c++20'],
  subproject_dir : 'lib')


add_project_arguments(['-Wno-changes-meaning'], language: 'cpp')



cmake = import('cmake')

spdlog_proj = cmake.subproject('spdlog')
spdlog_dep = spdlog_proj.dependency('spdlog')

qt6_dep = dependency('qt6', modules: ['Core', 'Sql', 'Gui', 'Widgets'])
qt6 = import('qt6')

subdir('src')
subdir('tests')

