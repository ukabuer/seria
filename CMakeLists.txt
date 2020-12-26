cmake_minimum_required(VERSION 3.11)

project(
    seria
    VERSION 0.1.0
    LANGUAGES CXX C
)

set(MASTER_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MASTER_PROJECT ON)
endif ()

option(SERIA_USE_EXTERNAL_RAPIDJSON "use external rapidjson" OFF)
option(SERIA_ENABLE_MPACK "enable mpack(msgpack) support" ON)
option(SERIA_USE_EXTERNAL_MPACK "use external mpack" OFF)
option(SERIA_BUILD_TESTS "whether to build tests" ${MASTER_PROJECT})
option(SERIA_INSTALL "whether to install seria" ${MASTER_PROJECT})
option(FETCHCONTENT_QUIET "" OFF)

if (UNIX)
  set(CMAKE_CXX_FLAGS "-Wall -Wextra")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=leak -fsanitize=undefined")
  set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address -fsanitize=leak -fsanitize=undefined")
endif ()

if (SERIA_ENABLE_MPACK AND NOT SERIA_USE_EXTERNAL_MPACK)
  include(${PROJECT_SOURCE_DIR}/third_party/mpack.cmake)
endif ()

add_library(seria INTERFACE)
add_library(seria::seria ALIAS seria)
target_include_directories(
    seria
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(seria INTERFACE cxx_std_14)
if (SERIA_USE_EXTERNAL_RAPIDJSON)
  target_compile_definitions(seria INTERFACE SERIA_USE_EXTERNAL_RAPIDJSON)
endif ()

if (SERIA_BUILD_TESTS)
  add_subdirectory(tests)
endif ()

if (SERIA_INSTALL)
  install(
      TARGETS seria
      EXPORT seria-targets
  )
  install(
      DIRECTORY
      ${PROJECT_SOURCE_DIR}/include/seria
      ${PROJECT_SOURCE_DIR}/include/rapidjson
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
endif ()