### Project ##################################################################

list(APPEND matching_HEADERS
  include/FileCache.h
  include/IMatcher.h
  include/Pcre2Matcher.h
  include/TextBuffer.h
  include/TextInfo.h
  include/TextUtil.h
  )

list(APPEND matching_SOURCES
  src/IMatcher.cpp
  src/IMatcherFactory.cpp
  src/Pcre2Matcher.cpp
  src/TextBuffer.cpp
  src/TextInfo.cpp
  )

### Target ###################################################################

add_library(matching STATIC
  ${matching_HEADERS}
  ${matching_SOURCES}
  )

format_output_name(matching "matching")

set_target_properties(matching PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  )

target_compile_definitions(matching
  PRIVATE PCRE2_CODE_UNIT_WIDTH=8
  )

target_include_directories(matching
  PRIVATE ${csFiles_BINARY_DIR}/3rdparty/pcre2-src
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

target_link_libraries(matching
  PRIVATE Qt5::Core
  PUBLIC  csUtil pcre2-8
  )
