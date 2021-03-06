cmake_minimum_required(VERSION 3.0)
project(PlycExamples C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -march=native)  # march=native for best performance
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-long-long -pedantic -Wno-unused-function -Wno-missing-braces")

# find plyc libs as PLYC_LIB
# (plyc should be compiled and installed, so that CMake can find and link to it)
find_library(PLYC_LIB plyc)

# add example executables
add_executable(helloplyc helloplyc.c)
add_executable(easysave easysave.c)
add_executable(mesh mesh.c)
add_executable(parseadvadvanced parsedadvanced.c)

# link to plyc
target_link_libraries(helloplyc ${PLYC_LIB})
target_link_libraries(easysave ${PLYC_LIB})
target_link_libraries(mesh ${PLYC_LIB})
target_link_libraries(parseadvadvanced ${PLYC_LIB})

# copy data
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

