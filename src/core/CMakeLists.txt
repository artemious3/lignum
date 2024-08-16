
file(GLOB data_model_SRC CONFIGURE_DEPENDS "gedcomx/*.cpp")

message(STATUS "${data_model_SRC}")

target_sources(mftb PRIVATE "${data_model_SRC}")
target_sources(mftb PRIVATE DB.cpp)

target_include_directories(mftb PRIVATE gedcomx)
target_include_directories(mftb PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})