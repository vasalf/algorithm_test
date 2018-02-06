add_library (speedtest STATIC
        include/speedtest/speedtest.h
        speedtest.cpp)
target_include_directories(speedtest PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../ascii_table/include)
target_link_libraries(speedtest LINK_PUBLIC
  ascii_table)
