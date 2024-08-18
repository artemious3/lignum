
add_library(mftbcore DBImpl.cpp datamodel.h)
target_link_libraries(mftbcore PUBLIC Qt6::Core Qt6::Sql)