cmake_minimum_required(VERSION 3.0)
project(Plyc C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -march=native)  # march=native for best performance
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-long-long -pedantic -Wno-unused-function -Wno-missing-braces")

# -fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(plyc
        src/types.c
        src/header_parse.c
        src/header_write.c
        src/data_parse.c
        src/data_write.c
        src/ply_load.c
        src/ply_save.c
        src/simple_load.c
        src/simple_save.c
        )

target_include_directories(plyc PUBLIC include)


#
# Testing
#

# enables ctest (after "make", test with "ctest" or "ctest -V")
enable_testing()

# define the executable test
add_executable(test_header tests/header_test.c)
add_executable(test_data tests/data_test.c)
add_executable(test_ply tests/ply_test.c)
add_executable(test_simple tests/simple_test.c)

# link to the library
target_link_libraries(test_header plyc)
target_link_libraries(test_data plyc)
target_link_libraries(test_ply plyc)
target_link_libraries(test_simple plyc)


# add the executable as test
add_test(TestHeader test_header)
add_test(TestData test_data)
add_test(TestPly test_ply)
add_test(TestSimple test_simple)

# copy data
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tests/data/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


#
# install:
#
install(TARGETS plyc
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )
install(DIRECTORY include/ DESTINATION include)
