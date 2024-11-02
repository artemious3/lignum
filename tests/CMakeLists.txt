find_package(GTest REQUIRED)

file(GLOB core_test CONFIGURE_DEPENDS "database/*.cpp" "app/*.cpp"  )

include_directories("${CMAKE_SOURCE_DIR}/src/database")
include_directories("${CMAKE_SOURCE_DIR}/src/app/family-tree")
include_directories("${CMAKE_SOURCE_DIR}/src/app/family-tree/default-theme")

add_executable(test_executable "${core_test}")
target_link_libraries(test_executable mftbdatabase GTest::gtest_main FamilyTreeItem )
add_test(google_tests test_executable)

