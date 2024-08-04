
file(GLOB data_model_SRC CONFIGURE_DEPENDS "data-model/*.cpp")

message(STATUS "${data_model_SRC}")

target_sources(mftb PRIVATE "${data_model_SRC}")
target_sources(mftb PRIVATE DB.cpp)

target_include_directories(mftb PRIVATE data-model)
target_include_directories(mftb PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})