cmake_minimum_required(VERSION 3.11)

project(
    seria
    VERSION 0.1.0
    LANGUAGES CXX
)

include(cmake/utils.cmake)
include(third_party/RapidJSON.cmake)

add_library(seria INTERFACE)
add_library(seria::seria ALIAS seria)
target_include_directories(
    seria
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${rapidjson_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(seria INTERFACE cxx_std_14)

set(MASTER_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MASTER_PROJECT ON)
endif ()

option(SERIA_BUILD_TESTS "whether to build tests" ${MASTER_PROJECT})
if (SERIA_BUILD_TESTS)
  add_subdirectory(tests)
endif ()

install(
    TARGETS seria
    EXPORT seria-targets
)
install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/seria
    DESTINATION include
)
install(
    EXPORT seria-targets
    FILE seria-targets.cmake
    NAMESPACE seria::
    DESTINATION lib/cmake/seria
)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/seria-config.cmake"
    INSTALL_DESTINATION "lib/cmake/seria"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/seria-config.cmake
    DESTINATION lib/cmake/seria
)

export(
    EXPORT seria-targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/seria-targets.cmake"
)
