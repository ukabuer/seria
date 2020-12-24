include(${PROJECT_SOURCE_DIR}/third_party/catch2.cmake)

add_executable(test_rapidjson rapidjson.cpp)
target_link_libraries(test_rapidjson PRIVATE seria::seria Catch2::Catch2WithMain)
target_compile_features(test_rapidjson PRIVATE cxx_std_14)

add_executable(test_mpack mpack.cpp)
target_link_libraries(test_mpack PRIVATE seria::seria Catch2::Catch2WithMain mpack)
target_compile_features(test_mpack PRIVATE cxx_std_14)

enable_testing()
add_test(NAME JSONTest COMMAND test_rapidjson)
add_test(NAME MsgPackTest COMMAND test_mpack)