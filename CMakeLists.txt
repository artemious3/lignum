cmake_minimum_required(VERSION 3.16)

project(tt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

qt_standard_project_setup()

include_directories(lib/spdlog/include)
include_directories(app/tree-wizard)
include_directories(core)


add_subdirectory(lib/spdlog)
add_subdirectory(src)
add_subdirectory(tests)




