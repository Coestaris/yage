cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME app)
set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/trivial_render_chain.cpp)

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

target_link_libraries(${PROJECT_NAME} PRIVATE yagecore yagepack)

file(GLOB_RECURSE ___src *.cpp *.hpp)
foreach (file ${___src})
    get_filename_component(filename ${file} NAME)
    set_source_files_properties(${file} PROPERTIES COMPILE_DEFINITIONS "__SHORT_FILE__=\"${filename}\";__USE_SHORT_FILE__=1")
endforeach ()