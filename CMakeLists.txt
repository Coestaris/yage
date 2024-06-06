cmake_minimum_required(VERSION 3.16)

project(yage LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-value -Wno-unused-local-typedefs")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-value -Wno-unused-local-typedefs")

set(SRC_FILES
        src/*.cpp
)

foreach (src_file ${SRC_FILES})
    file(GLOB_RECURSE src_files ${src_file})
    list(APPEND PLAIN_SRC_FILES ${src_files})
endforeach ()

add_executable(yage ${PLAIN_SRC_FILES})