qt_add_executable(mftb)

file(GLOB tree_wizard_SRC CMAKE_CONFIGURE_DEPENDS "mftb-window/*.cpp")
file(GLOB tree_scene_SRC CMAKE_CONFIGURE_DEPENDS "mftb-window/tree-scene/*.cpp")


target_sources(mftb PRIVATE
 ${tree_wizard_SRC}
 ${tree_scene_SRC}
 main.cpp
 mftb-window/mftbwindow.ui)

 target_include_directories(mftb PRIVATE mftb-window)
 target_include_directories(mftb PRIVATE mftb-window/tree-scene)

 add_subdirectory(res)


target_link_libraries(mftb PRIVATE Qt6::Core Qt6::Widgets mftbcore)
