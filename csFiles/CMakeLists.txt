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

set(PCRE2_BUILD_PCRE2_8   ON  CACHE BOOL "" FORCE)
set(PCRE2_DEBUG           OFF CACHE BOOL "" FORCE)
set(PCRE2_SUPPORT_JIT     ON  CACHE BOOL "" FORCE)
set(PCRE2_BUILD_TESTS     OFF CACHE BOOL "" FORCE)
set(PCRE2_BUILD_PCRE2GREP OFF CACHE BOOL "" FORCE)

add_subdirectory(3rdparty/pcre2-src)

### Project ##################################################################

list(APPEND csFiles_FORMS
  forms/WFind.ui
  forms/WGrep.ui
  forms/WMainWindow.ui
  )

list(APPEND csFiles_HEADERS
  include/FileCache.h
  include/FilesModel.h
#  include/IMatcher.h
#  include/Pcre2Matcher.h
  include/TextBuffer.h
  include/TextInfo.h
  include/TextUtil.h
  include/WFileList.h
  include/WFind.h
  include/WGrep.h
  include/WMainWindow.h
  )

list(APPEND csFiles_SOURCES
  src/FilesModel.cpp
#  src/IMatcher.cpp
#  src/Pcre2Matcher.cpp
  src/TextBuffer.cpp
  src/TextInfo.cpp
  src/WFileList.cpp
  src/WFind.cpp
  src/WGrep.cpp
  src/WMainWindow.cpp
  src/main.cpp
  )

### Target ###################################################################

add_executable(csFiles
  ${csFiles_FORMS}
  ${csFiles_HEADERS}
  ${csFiles_SOURCES}
  )

format_output_name(csFiles "csFiles")

set_target_properties(csFiles PROPERTIES
  AUTOMOC ON
  AUTORCC ON
  AUTOUIC ON
  AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms
  )

if(WIN32)
  set_target_properties(csFiles PROPERTIES
    WIN32_EXECUTABLE ON
    )
endif()

target_compile_definitions(csFiles
  PRIVATE -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII
  )

target_include_directories(csFiles PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

target_link_libraries(csFiles csQt Qt5::Widgets)
