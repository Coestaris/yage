cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME app)
set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})

add_executable(${PROJECT_NAME})

set(SRC_FILES
        ${PROJECT_DIR}/src/objects/*.cpp
        ${PROJECT_DIR}/src/*.cpp
)

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

target_link_libraries(${PROJECT_NAME} PRIVATE yagecore yagepack)

foreach (src_file ${SRC_FILES})
    file(GLOB src_files ${src_file})
    list(APPEND PLAIN_SRC_FILES ${src_files})
endforeach ()
target_sources(${PROJECT_NAME} PRIVATE ${PLAIN_SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_DIR}/src)

if (WIN32)
    enable_language(RC)
    target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_DIR}/src/app.rc)
endif ()
if (WIN32 AND ${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set_target_properties(${PROJECT_NAME} PROPERTIES
            WIN32_EXECUTABLE TRUE
            LINK_FLAGS "-mwindows"
    )
endif ()

file(GLOB_RECURSE ___src *.cpp *.hpp)
foreach (file ${___src})
    get_filename_component(filename ${file} NAME)
    set_source_files_properties(${file} PROPERTIES COMPILE_DEFINITIONS "__SHORT_FILE__=\"${filename}\";__USE_SHORT_FILE__=1")
endforeach ()