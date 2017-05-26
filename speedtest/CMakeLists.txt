add_library (speedtest STATIC
  speedtest.cpp)
target_include_directories(speedtest PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
