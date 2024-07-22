cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME yagepack)
set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(THIRDPARTY_DIR ${PROJECT_DIR}/thirdparty)

if (SINGLEBIN)
    set(BUILD_SHARED_LIBS OFF)
else ()
    set(BUILD_SHARED_LIBS ON)
endif ()
add_library(${PROJECT_NAME} ${sources})

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
target_compile_options(${PROJECT_NAME} PRIVATE ${YAGE_CXX_COMPILE_FLAGS} ${cxx_flags})
target_link_options(${PROJECT_NAME} PRIVATE ${YAGE_CXX_LINK_FLAGS})

target_sources(${PROJECT_NAME} PRIVATE
    src/asset_factory.cpp
    include/yagepack/asset_factory.hpp
)

if (WIN32 AND NOT SINGLEBIN)
    enable_language(RC)
    target_sources(${PROJECT_NAME} PRIVATE src/yagepack.rc)
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC
    include
)
target_link_libraries(${PROJECT_NAME} PRIVATE yagecore)

file(GLOB_RECURSE ___src *.cpp *.hpp)
foreach (file ${___src})
    get_filename_component(filename ${file} NAME)
    set_source_files_properties(${file} PROPERTIES COMPILE_DEFINITIONS "__SHORT_FILE__=\"${filename}\";__USE_SHORT_FILE__=1")
endforeach ()