#qt_add_executable(mftb)

file(GLOB main_window_SRC CMAKE_CONFIGURE_DEPENDS "mftb-window/*.cpp")
file(GLOB family_tree_item_SRC CMAKE_CONFIGURE_DEPENDS "family-tree-item/*.cpp")

add_library(FamilyTreeItem "${family_tree_item_SRC}")
target_link_libraries(FamilyTreeItem PUBLIC Qt6::Core Qt6::Widgets mftbdatabase)

# target_sources(mftb PRIVATE
#  ${tree_wizard_SRC}
#  main.cpp
#  mftb-window/mftbwindow.ui)

#  include_directories(mftb PRIVATE mftb-window)
#  include_directories(mftb PRIVATE mftb-window/tree-scene)
include_directories(mftb PRIVATE "${CMAKE_SOURCE_DIR}/src/database")

# add_subdirectory(res)

# target_link_libraries(mftb PRIVATE Qt6::Core Qt6::Widgets mftbdatabase FamilyTreeItem)
