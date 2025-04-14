
add_library(mftbdatabase SqlDB.cpp)
target_link_libraries(mftbdatabase PUBLIC Qt6::Core Qt6::Sql)
