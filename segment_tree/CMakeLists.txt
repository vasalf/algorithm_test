add_executable (segment_tree
  main.cpp)
target_include_directories (segment_tree PUBLIC
  ../speedtest/include
  include)
target_link_libraries (segment_tree
  speedtest)

add_executable (segment_tree_corr
  corr.cpp)
target_include_directories (segment_tree_corr PUBLIC
  ../speedtest/include
  include)
target_link_libraries (segment_tree_corr
  speedtest)
