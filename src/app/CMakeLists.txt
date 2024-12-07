

file(GLOB main_window_SRC CMAKE_CONFIGURE_DEPENDS "main-window/*.cpp")
file(GLOB renderer_SRC CMAKE_CONFIGURE_DEPENDS "renderer/*.cpp")
file(GLOB family_tree_item_SRC CMAKE_CONFIGURE_DEPENDS "family-tree/*.cpp")
file(GLOB default_theme_SRC CMAKE_CONFIGURE_DEPENDS "family-tree/default-theme/*.cpp")
file(GLOB config_SRC CONFIGURE_DEPENDS "config/*.cpp")

add_library(FamilyTreeItem "${family_tree_item_SRC};${renderer_SRC}")
target_link_libraries(FamilyTreeItem PUBLIC Qt6::Core Qt6::Widgets mftbdatabase)

qt_add_executable(mftb)

include_directories(mftb PRIVATE main-window)
include_directories(mftb PRIVATE renderer)
include_directories(mftb PRIVATE config)
include_directories(mftb PRIVATE family-tree)
include_directories(mftb PRIVATE family-tree/default-theme)
include_directories(mftb PRIVATE "${CMAKE_SOURCE_DIR}/src/database")

target_sources(mftb PRIVATE
 ${main_window_SRC}
 ${config_SRC}
 ${default_theme_SRC}
 main.cpp
 main-window/mftbwindow.ui)




add_subdirectory(res)

target_link_libraries(mftb PRIVATE Qt6::Core Qt6::Widgets mftbdatabase FamilyTreeItem spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
