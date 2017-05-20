add_library (ascii_table STATIC
  table.cpp
  column.cpp
  cell.cpp)

target_include_directories (ascii_table PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
