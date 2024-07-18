add_subdirectory(${PROJECT_DIR}/thirdparty/freetype)

if (NOT YAGECORE_STATIC)
    set(BUILD_SHARED_LIBS ON)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_DIR}/thirdparty/freetype/include)
target_link_libraries(${PROJECT_NAME} PRIVATE freetype)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(freetype PRIVATE -g3 -O0)
    target_link_options(freetype PRIVATE -g3 -O0)
else ()
    target_compile_options(freetype PRIVATE -g0 -O2 -s)
    target_link_options(freetype PRIVATE -g0 -O2 -s)
endif ()

if (NOT YAGECORE_STATIC)
    target_compile_options(freetype PRIVATE -fPIC)
endif()
