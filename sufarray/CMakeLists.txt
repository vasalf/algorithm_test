add_executable (sufarray
    main.cpp
    include/tests/random_str.h
    include/tests/ordered.h
    include/tests/reversed.h
    include/solutions/nlog2_hashes.h
    include/solutions/nlogn.h
)

target_include_directories (sufarray PUBLIC
    ../speedtest/include
    include
)

#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVERIFY")
target_link_libraries (sufarray speedtest)
