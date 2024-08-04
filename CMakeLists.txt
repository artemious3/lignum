cmake_minimum_required(VERSION 3.16)

project(tt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt6 REQUIRED COMPONENTS Core Core5Compat Widgets)


qt_standard_project_setup()

qt_add_executable(mftb)

include_directories(app/tree-wizard)
include_directories(core)

#include_directories(lib/QtJsonSerializer/include)
#include_directories(lib/QtJsonSerializer/include/QtJsonSerializer)
#link_directories(lib)

add_subdirectory(src)
add_subdirectory(res)

target_link_libraries(mftb PRIVATE Qt6::Core Qt6::Widgets Qt5JsonSerializer)

