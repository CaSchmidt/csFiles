### Project ##################################################################

list(APPEND find_HEADERS
  include/ExtensionFilter.h
  include/FilenameFilter.h
  include/IFindFilter.h
  include/PathFilter.h
  include/PatternList.h
  )

list(APPEND find_SOURCES
  src/ExtensionFilter.cpp
  src/FilenameFilter.cpp
  src/IFindFilter.cpp
  src/PathFilter.cpp
  src/PatternList.cpp
  )

### Target ###################################################################

add_library(find STATIC
  ${find_HEADERS}
  ${find_SOURCES}
  )

format_output_name(find "find")

target_include_directories(find
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

target_link_libraries(find
  PRIVATE Qt5::Core
  )
