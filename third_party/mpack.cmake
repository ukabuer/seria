include(FetchContent)
FetchContent_Declare(
    mpack
    GIT_REPOSITORY https://github.com/ludocode/mpack.git
    GIT_TAG v1.0
    GIT_SHALLOW true
    GIT_PROGRESS TRUE
)

FetchContent_GetProperties(mpack)
if (NOT mpack_POPULATED)
  FetchContent_Populate(mpack)
endif ()

FetchContent_GetProperties(
    mpack
    SOURCE_DIR mpack_src
)

aux_source_directory(${mpack_src}/src/mpack SRC)
add_library(mpack STATIC ${SRC})
target_include_directories(mpack PUBLIC ${mpack_src}/src)
