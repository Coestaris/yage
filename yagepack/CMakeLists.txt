cmake_minimum_required(VERSION 3.16)

option(YAGEPACK_STATIC "Build yagepack as a static library" OFF)

set(PROJECT_NAME yagepack)
set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})

if (YAGEPACK_STATIC)
    add_library(${PROJECT_NAME} STATIC
            ${sources}
    )
else()
    add_library(yagepack SHARED
            ${sources}
    )
endif()

# Here, to not affect the build process by our options
include(cmake/jsoncpp.cmake)
include(cmake/elzip.cmake)

# Setup compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(cxx_flags
    -Wall -Wextra -Werror -lpthread
    -Wno-unused-parameter -Wno-unused-variable
    -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-private-field
    -Wno-unused-const-variable -Wno-unused-value -Wno-unused-local-typedefs -Werror=old-style-cast
)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -g3 -O0 -DUSE_DEBUG ${cxx_flags})
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -O3 ${cxx_flags})
endif ()

target_sources(${PROJECT_NAME} PRIVATE
    src/asset_factory.cpp
    include/yagepack/asset_factory.hpp
)
target_include_directories(${PROJECT_NAME} PUBLIC
    include
)
target_link_libraries(${PROJECT_NAME} PRIVATE yagecore)
