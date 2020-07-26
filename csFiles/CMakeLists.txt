cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

### Project ##################################################################

project(csFiles)

### Modules ##################################################################

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(FormatOutputName)

### Dependencies #############################################################

find_package(Qt5Widgets 5.6 REQUIRED)

add_subdirectory(../csQt/csQt
  ${CMAKE_CURRENT_BINARY_DIR}/csQt
  )

add_subdirectory(../csUtil/csUtil
  ${CMAKE_CURRENT_BINARY_DIR}/csUtil
  )

set(PCRE2_BUILD_PCRE2_8   ON  CACHE BOOL "" FORCE)
set(PCRE2_DEBUG           OFF CACHE BOOL "" FORCE)
set(PCRE2_SUPPORT_JIT     ON  CACHE BOOL "" FORCE)
set(PCRE2_BUILD_TESTS     OFF CACHE BOOL "" FORCE)
set(PCRE2_BUILD_PCRE2GREP OFF CACHE BOOL "" FORCE)

add_subdirectory(3rdparty/pcre2-src)

### Subdirectories ###########################################################

add_subdirectory(matching)
add_subdirectory(ui)
