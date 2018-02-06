add_executable (rope
        main.cpp
        include/solutions/treap.h
        include/solutions/splay.h
        include/tests/build_insert_erase.h
        include/tests/build_long_struct.h
        include/tests/build_shuffle.h
        include/tests/insert_erase.h include/solutions/verify.h include/solutions/avl.h)
target_include_directories (rope PUBLIC
  ../speedtest/include
  include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVERIFY")
target_link_libraries (rope
  speedtest)