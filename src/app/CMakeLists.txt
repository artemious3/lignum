

file(GLOB main_window_SRC CMAKE_CONFIGURE_DEPENDS "main-window/*.cpp")
file(GLOB balancer_SRC CMAKE_CONFIGURE_DEPENDS "balancer/*.cpp")
file(GLOB family_tree_item_SRC CMAKE_CONFIGURE_DEPENDS "family-tree/*.cpp")
file(GLOB config_SRC CONFIGURE_DEPENDS "config/*.cpp")

add_library(FamilyTreeItem "${family_tree_item_SRC};${balancer_SRC}")
target_link_libraries(FamilyTreeItem PUBLIC Qt6::Core Qt6::Widgets mftbdatabase)


qt_add_executable(mftb)

target_sources(mftb PRIVATE
 ${main_window_SRC}
 ${config_SRC}
 main.cpp
 main-window/mftbwindow.ui)

 include_directories(mftb PRIVATE main-window)
 include_directories(mftb PRIVATE balancer)
 include_directories(mftb PRIVATE config)


include_directories(mftb PRIVATE "${CMAKE_SOURCE_DIR}/src/database")
include_directories(mftb PRIVATE "${CMAKE_SOURCE_DIR}/src/app/family-tree")

add_subdirectory(res)

target_link_libraries(mftb PRIVATE Qt6::Core Qt6::Widgets mftbdatabase FamilyTreeItem spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
