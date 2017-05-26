add_executable (aplusb
  main.cpp)
target_include_directories(aplusb PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../speedtest/include)
target_link_libraries(aplusb LINK_PUBLIC speedtest)
