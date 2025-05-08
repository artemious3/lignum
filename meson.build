
project('lignum', 'cpp', 
  default_options : ['cpp_std=c++20'],
  subproject_dir : 'lib')

compiler_flags = ['-Wno-changes-meaning']
if get_option('buildtype') == 'debug' 
  compiler_flags += ['-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE']
endif
add_project_arguments(compiler_flags, language: 'cpp')

cmake = import('cmake')

spdlog_proj = cmake.subproject('spdlog')
spdlog_dep = spdlog_proj.dependency('spdlog')


gedcom_proj = subproject('gedcom')
gedcom_dep =  gedcom_proj.get_variable('gedcom_dep')

qt6_dep = dependency('qt6', modules: ['Core', 'Sql', 'Gui', 'Widgets'])
qt6 = import('qt6')


subdir('src')
subdir('tests')

