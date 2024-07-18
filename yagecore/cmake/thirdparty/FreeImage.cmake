if (NOT YAGECORE_STATIC)
    set(BUILD_SHARED_LIBS ON)
endif()

add_definitions(-DFREEIMAGE_LIB)
add_subdirectory(${PROJECT_DIR}/thirdparty/FreeImage)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_DIR}/thirdparty/FreeImage/Source)
target_link_libraries(${PROJECT_NAME} PRIVATE FreeImage::FreeImage)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(FreeImage PRIVATE -g3 -O0)
    target_link_options(FreeImage PRIVATE -g3 -O0)
else ()
    target_compile_options(FreeImage PRIVATE -g0 -O2 -s)
    target_link_options(FreeImage PRIVATE -g0 -O2 -s)
endif ()

if (NOT YAGECORE_STATIC)
    target_compile_options(FreeImage PRIVATE -fPIC)
endif()
