cmake_minimum_required(VERSION 3.0)
project(Plyc C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -march=native)  # march=native for best performance
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-long-long -pedantic -Wno-unused-function -Wno-missing-braces")


add_library(plyc
        src/rcodes.c
        src/header_parse.c
        src/header_write.c
        src/data_get.c
        src/data_parse.c
        src/data_write.c
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
add_executable(main tests/main.c)
add_executable(test_header tests/header_test.c)
add_executable(test_data tests/data_test.c)
add_executable(test_simple tests/simple_test.c)

# link to the library
target_link_libraries(main plyc)
target_link_libraries(test_header plyc)
target_link_libraries(test_data plyc)
target_link_libraries(test_simple plyc)


# add the executable as test
add_test(TestHeader test_header)
add_test(TestData test_data)
add_test(TestSimple test_simple)

# copy data
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tests/data/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


#
# Examples
#

add_executable(helloplyc examples/helloplyc.c)
add_executable(easysave examples/easysave.c)
add_executable(mesh examples/mesh.c)

# link to the library
target_link_libraries(helloplyc plyc)
target_link_libraries(easysave plyc)
target_link_libraries(mesh plyc)


# copy data
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/examples/data/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})