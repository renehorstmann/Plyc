cmake_minimum_required(VERSION 3.0)
project(Plyc C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -march=native)  # march=native for best performance
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-long-long -pedantic -Wno-unused-function -Wno-missing-braces")


add_library(plyc
        src/rcodes.c
        src/plyheader.c
        )

target_include_directories(plyc PUBLIC include)


#
# Testing
#

# Enables ctest (after "make", test with "ctest" or "ctest -V")
enable_testing()

# Define the executable test
add_executable(main tests/main.c)
add_executable(testheader tests/headertest.c)

# link to the library
target_link_libraries(main plyc)
target_link_libraries(testheader plyc)


# add the executable as test
add_test(TestHeader testheader)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tests/data/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
