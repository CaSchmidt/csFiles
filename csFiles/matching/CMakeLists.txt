### Project ##################################################################

list(APPEND matching_HEADERS
  include/FileCache.h
#  include/IMatcher.h
#  include/Pcre2Matcher.h
  include/TextBuffer.h
  include/TextInfo.h
  include/TextUtil.h
  )

list(APPEND matching_SOURCES
#  src/IMatcher.cpp
#  src/Pcre2Matcher.cpp
  src/TextBuffer.cpp
  src/TextInfo.cpp
  )

### Target ###################################################################

add_library(matching STATIC
  ${matching_HEADERS}
  ${matching_SOURCES}
  )

target_include_directories(matching
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

target_link_libraries(matching
  PRIVATE Qt5::Core
  )
