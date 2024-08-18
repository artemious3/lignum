find_package(GTest REQUIRED)

file(GLOB core_test CONFIGURE_DEPENDS "core/*.cpp")

include_directories("${CMAKE_SOURCE_DIR}/src/database")
add_executable(test_executable "${core_test}")
target_link_libraries(test_executable mftbdatabase GTest::gtest_main )
add_test(google_tests test_executable)

