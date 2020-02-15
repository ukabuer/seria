find_package(RapidJSON CONFIG REQUIRED)
add_executable(test main.cpp)

target_include_directories(
    test
    PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
    ${RAPIDJSON_INCLUDE_DIRS}
)
target_compile_features(test PRIVATE cxx_std_14)