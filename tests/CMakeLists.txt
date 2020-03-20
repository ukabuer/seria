add_executable(test main.cpp)
target_include_directories(test PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(test PRIVATE seria::seria)
target_compile_features(test PRIVATE cxx_std_14)