include(${PROJECT_SOURCE_DIR}/third_party/catch2.cmake)

add_executable(test main.cpp)
target_link_libraries(test PRIVATE seria::seria Catch2::Catch2WithMain)
target_compile_features(test PRIVATE cxx_std_14)

enable_testing()
add_test(NAME BasicTest COMMAND test)