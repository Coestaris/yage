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
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -g3 -O0 -DCOLLECT_DRAW_STATISTICS=1 -DUSE_DEBUG ${cxx_flags})
    target_link_options(${PROJECT_NAME} PRIVATE -g3 -O0)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -g0 -O2 -s ${cxx_flags})
    target_link_options(${PROJECT_NAME} PRIVATE -g0 -O2 -s)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE yagecore yagepack)

foreach (src_file ${SRC_FILES})
    file(GLOB src_files ${src_file})
    list(APPEND PLAIN_SRC_FILES ${src_files})
endforeach ()
target_sources(${PROJECT_NAME} PRIVATE ${PLAIN_SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_DIR}/src)

file(GLOB_RECURSE ___src *.cpp *.hpp)
foreach (file ${___src})
    get_filename_component(filename ${file} NAME)
    set_source_files_properties(${file} PROPERTIES COMPILE_DEFINITIONS "__SHORT_FILE__=\"${filename}\";__USE_SHORT_FILE__=1")
endforeach ()

if (WIN32)
    # Find windres
    find_program(WINRES_EXECUTABLE NAMES windres REQUIRED)

    # Add resource file
    set(RESOURCE_FILE ${PROJECT_DIR}/src/app.rc)
    add_custom_command(
            OUTPUT ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.res
            COMMAND ${WINRES_EXECUTABLE} -O coff -I ${PROJECT_DIR}/src -i ${RESOURCE_FILE} -o ${PROJECT_NAME}.res
            MAIN_DEPENDENCY ${RESOURCE_FILE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Building resource file"
    )

    # Add the compiled resource file to the executable
    target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.res)

    target_link_options(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}.res)

    # Specify that the executable is a Windows GUI application
    set_target_properties(${PROJECT_NAME} PROPERTIES
            WIN32_EXECUTABLE TRUE
            LINK_FLAGS "-mwindows"
    )

endif ()