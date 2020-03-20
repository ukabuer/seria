option(RAPIDJSON_BUILD_DOC "Build rapidjson documentation." OFF)
option(RAPIDJSON_BUILD_EXAMPLES "Build rapidjson examples." OFF)
option(RAPIDJSON_BUILD_TESTS "Build rapidjson perftests and unittests." OFF)

include(FetchContent)
FetchContent_Declare(
    RapidJSON
    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
    GIT_TAG 814bb27bf0f1e3e924fc5c8e374dedf85c9cc70a
)
FetchContent_MakeAvailable(RapidJSON)