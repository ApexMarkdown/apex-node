cmake_minimum_required(VERSION 3.15)
project(apex_node LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(APEX_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../vendor/apex")

set(CMARK_TESTS OFF CACHE BOOL "" FORCE)
set(CMARK_SHARED OFF CACHE BOOL "" FORCE)
set(CMARK_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory("${APEX_ROOT}/vendor/cmark-gfm" "${CMAKE_BINARY_DIR}/cmark-gfm")

set(YAML_SRC
  ${APEX_ROOT}/vendor/libyaml/src/api.c
  ${APEX_ROOT}/vendor/libyaml/src/dumper.c
  ${APEX_ROOT}/vendor/libyaml/src/emitter.c
  ${APEX_ROOT}/vendor/libyaml/src/loader.c
  ${APEX_ROOT}/vendor/libyaml/src/parser.c
  ${APEX_ROOT}/vendor/libyaml/src/reader.c
  ${APEX_ROOT}/vendor/libyaml/src/scanner.c
  ${APEX_ROOT}/vendor/libyaml/src/writer.c
)
add_library(apex_yaml STATIC ${YAML_SRC})
target_include_directories(apex_yaml PUBLIC
  ${APEX_ROOT}/vendor/libyaml/include
  ${APEX_ROOT}/vendor/libyaml/src
)
target_compile_definitions(apex_yaml PUBLIC
  YAML_VERSION_MAJOR=0
  YAML_VERSION_MINOR=2
  YAML_VERSION_PATCH=5
  YAML_VERSION_STRING="0.2.5"
  HAVE_CONFIG_H=0
)

# Compile the same library sources as Apex's own build, so files added in new
# Apex releases are picked up without editing this list.
file(READ "${APEX_ROOT}/CMakeLists.txt" APEX_CMAKE_CONTENTS)
string(REGEX MATCH "set\\(APEX_LIB_SOURCES[^)]*\\)" APEX_SOURCES_BLOCK "${APEX_CMAKE_CONTENTS}")
if(NOT APEX_SOURCES_BLOCK)
  message(FATAL_ERROR "APEX_LIB_SOURCES not found in ${APEX_ROOT}/CMakeLists.txt")
endif()
string(REGEX MATCHALL "src/[^ \t\r\n)]+\\.c" APEX_LIB_SOURCES "${APEX_SOURCES_BLOCK}")
list(TRANSFORM APEX_LIB_SOURCES PREPEND "${APEX_ROOT}/")

add_library(apex_static STATIC ${APEX_LIB_SOURCES})
target_include_directories(apex_static PUBLIC
  ${APEX_ROOT}/include
  # NOTE: intentionally NOT adding ${APEX_ROOT}/include/apex here. Apex's own
  # sources include it as "apex/apex.h" and its sibling headers resolve via
  # the including file's own directory. Adding include/apex directly would
  # expose a bare "parser.h" (apex's AST parser header) that shadows
  # cmark-gfm's internal src/parser.h of the same name, which several Apex
  # extensions (e.g. wiki_links.c) need for the full cmark_parser struct.
  ${APEX_ROOT}/vendor/cmark-gfm/src
  ${APEX_ROOT}/vendor/cmark-gfm/extensions
  ${APEX_ROOT}/vendor/libyaml/include
  # cmark-gfm's generate_export_header() writes cmark-gfm_export.h here.
  ${CMAKE_BINARY_DIR}/cmark-gfm/src
)
target_compile_definitions(apex_static PUBLIC APEX_HAVE_LIBYAML=1)
target_link_libraries(apex_static PUBLIC
  libcmark-gfm-extensions_static
  libcmark-gfm_static
  apex_yaml
)

add_library(${PROJECT_NAME} SHARED
  src/addon.cpp
  src/options.cpp
)
set_target_properties(${PROJECT_NAME} PROPERTIES
  PREFIX ""
  SUFFIX ".node"
)
execute_process(
  COMMAND node -p "require('node-addon-api').include.replace(/\"/g, '')"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
  OUTPUT_VARIABLE NODE_ADDON_API_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_JS_INC}
  ${NODE_ADDON_API_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(${PROJECT_NAME} PRIVATE apex_static ${CMAKE_JS_LIB})
target_compile_definitions(${PROJECT_NAME} PRIVATE NAPI_VERSION=8)
