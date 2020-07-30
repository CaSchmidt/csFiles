### Project ##################################################################

list(APPEND ui_FORMS
  forms/WFind.ui
  forms/WGrep.ui
  forms/WMainWindow.ui
  forms/WProgressLogger.ui
  )

list(APPEND ui_HEADERS
  include/FilesModel.h
  include/ITabWidget.h
  include/MatchJob.h
  include/WFileList.h
  include/WFind.h
  include/WGrep.h
  include/WMainWindow.h
  include/WProgressLogger.h
  )

list(APPEND ui_SOURCES
  src/FilesModel.cpp
  src/ITabWidget.cpp
  src/MatchJob.cpp
  src/WFileList.cpp
  src/WFind.cpp
  src/WGrep.cpp
  src/WMainWindow.cpp
  src/WProgressLogger.cpp
  src/main.cpp
  )

### Target ###################################################################

add_executable(ui
  ${ui_FORMS}
  ${ui_HEADERS}
  ${ui_SOURCES}
  )

format_output_name(ui "csFiles")

set_target_properties(ui PROPERTIES
  AUTOMOC ON
  AUTORCC ON
  AUTOUIC ON
  AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms
  )

if(WIN32)
  set_target_properties(ui PROPERTIES
    WIN32_EXECUTABLE ON
    )
endif()

target_compile_definitions(ui
  PRIVATE -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII
  )

target_include_directories(ui PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

target_link_libraries(ui matching csQt Qt5::Concurrent Qt5::Widgets)
